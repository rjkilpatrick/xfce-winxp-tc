#include <gdk/gdk.h>
#include <gio/gdesktopappinfo.h>
#include <glib.h>

#include "exec.h"

//
// FORWARD DECLARATIONS
//
static gchar* expand_desktop_entry_cmdline(
    const gchar* cmdline,
    const gchar* name,
    const gchar* icon_name,
    const gchar* entry_path,
    gboolean     needs_terminal
);

static gchar* parse_url_in_cmdline(
    const gchar* cmdline,
    GError**     out_error
);

static void set_display(
    const gchar* display
);

static gchar** true_shell_parse_argv(
    const gchar* cmdline,
    GError**     out_error
);

//
// PUBLIC FUNCTIONS
//
gchar* wintc_desktop_app_info_get_command(
    GDesktopAppInfo* entry
)
{
    GAppInfo* app_info = G_APP_INFO(entry);

    const gchar* cmd_line = g_app_info_get_commandline(app_info);
    const gchar* exe_path = g_app_info_get_executable(app_info);

    if (cmd_line != NULL)
    {
        gchar* expanded;
        gchar* icon_name = g_path_get_basename(exe_path);

        expanded =
            expand_desktop_entry_cmdline(
                cmd_line,
                g_app_info_get_name(app_info),
                icon_name,
                g_desktop_app_info_get_filename(entry),
                FALSE
            );

        g_free(icon_name);

        return expanded;
    }
    else
    {
        return g_strdup(exe_path);
    }
}

gboolean wintc_launch_command(
    const gchar* cmdline,
    GError**     out_error
)
{
    if (out_error != NULL) { *out_error = NULL; }

    gchar**  argv;
    gchar*   display;
    GError*  error        = NULL;
    gchar*   real_cmdline;
    gboolean success      = FALSE;

    // Parse input in case it's a URL - it should be set as an argument to the scheme
    // handler
    //
    real_cmdline = parse_url_in_cmdline(cmdline, &error);

    if (real_cmdline == NULL)
    {
        if (out_error != NULL) { *out_error = g_error_copy(error); }

        g_clear_error(&error);
        return FALSE;
    }

    // Parse the command line into an argument vector
    //
    argv = true_shell_parse_argv(real_cmdline, &error);
    g_free(real_cmdline);

    if (argv == NULL)
    {
        if (out_error != NULL) { *out_error = g_error_copy(error); }
        
        g_error_free(error);
        return FALSE;
    }

    // Launch now!
    //
    display =
        g_strdup(
            gdk_display_get_name(gdk_display_get_default())
        );

    success =
        g_spawn_async(
            NULL,
            argv,
            NULL,
            0,
            (GSpawnChildSetupFunc) set_display,
            display,
            NULL,
            &error
        );

    g_free(display);
    g_strfreev(argv);

    if (!success)
    {
        if (out_error != NULL) { *out_error = g_error_copy(error); }

        g_error_free(error);
        return FALSE;
    }

    return TRUE;
}

gchar* wintc_query_mime_handler(
    const gchar*      mime_query,
    GError**          out_error,
    GDesktopAppInfo** out_entry
)
{
    g_message("Hello!");
    gchar* xdg_query_cmd =
        g_strconcat(
            "xdg-mime query default ",
            mime_query,
            NULL
        );

    GDesktopAppInfo* entry      = NULL;
    gchar*           cmd_output = NULL;
    GError*          error      = NULL;
    gchar*           filename   = NULL;
    gboolean         success    = FALSE;

    if (out_error != NULL) { *out_error = NULL; }
    if (out_entry != NULL) { *out_entry = NULL; }

    success =
        g_spawn_command_line_sync(
            xdg_query_cmd,
            &cmd_output,
            NULL,
            NULL,
            &error
        );

    g_free(xdg_query_cmd);

    if (success)
    {
        filename = g_utf8_substring(
                       cmd_output,
                       0,
                       g_utf8_strlen(cmd_output, -1) - 1
                   );
        entry    = g_desktop_app_info_new(
                       filename
                   );

        g_free(filename);
    }
    else
    {
        if (out_error != NULL) { *out_error = error;}

        g_free(cmd_output);

        return NULL;
    }

    if (entry == NULL)
    {
        return "";
    }

    if (out_entry != NULL) { *out_entry = entry; }

    return wintc_desktop_app_info_get_command(entry);
}

//
// PRIVATE FUNCTIONS
//
static gchar* expand_desktop_entry_cmdline(
    const gchar* cmdline,
    const gchar* name,
    const gchar* icon_name,
    const gchar* entry_path,
    gboolean     needs_terminal
)
{
    GString* expanded = g_string_sized_new(250);

    if (needs_terminal)
    {
        g_string_append(
            expanded,
            "exo-open --launch TerminalEmulator"
        );
    }

    // Iterate through cmdline character by character to expand shortcodes
    //
    const gchar* iter;

    for (iter = cmdline; *iter != '\0'; iter++)
    {
        if (
            iter[0] == '%' &&
            iter[1] != '\0'
        )
        {
            switch (*++iter)
            {
                case 'c':
                    if (name != NULL)
                    {
                        g_string_append(
                            expanded,
                            name
                        );
                    }

                    break;

                case 'i':
                    if (icon_name != NULL)
                    {
                        g_string_append(
                            expanded,
                            name
                        );
                    }

                    break;

                case 'k':
                    g_string_append(
                        expanded,
                        entry_path
                    );
                    break;

                case '%':
                    g_string_append_c(expanded, '%');
                    break;
            }
        }
        else
        {
            g_string_append_c(expanded, *iter);
        }
    }

    return g_string_free(expanded, FALSE);
}

static gchar* parse_url_in_cmdline(
    const gchar* cmdline,
    GError**     out_error
)
{
    static GRegex* url_regex = NULL;

    if (url_regex == NULL)
    {
        url_regex =
            g_regex_new(
                "^([A-Za-z-]+)://",
                0,
                0,
                NULL // TODO: Handle errors
            );
    }

    gchar*      app_name;
    GString*    final_cmdline;
    GMatchInfo* match_info;
    gchar*      mime_type;
    gchar*      uri_scheme;

    g_regex_match(url_regex, cmdline, 0, &match_info);

    // Command line isn't a URL, return a duplicate of the original
    //
    if (g_match_info_get_match_count(match_info) == 0)
    {
        g_match_info_free(match_info);
        return g_strdup(cmdline);
    }

    // Command line IS a URL, retrieve the scheme, query the handling program, return
    // program with URI as argument
    //
    uri_scheme = g_match_info_fetch(match_info, 1);
    mime_type  = g_strconcat(
                     "x-scheme-handler/",
                     uri_scheme,
                     NULL
                 );

    app_name = wintc_query_mime_handler(mime_type, out_error, NULL);

    g_match_info_free(match_info);

    if (app_name == NULL)
    {
        return NULL;
    }

    final_cmdline = g_string_sized_new(500);

    g_string_append(final_cmdline, app_name);
    g_string_append(final_cmdline, " ");
    g_string_append(final_cmdline, cmdline);

    return g_string_free(final_cmdline, FALSE);
}

static void set_display(
    const gchar* display
)
{
    g_setenv("DISPLAY", display, TRUE);
}

static gchar** true_shell_parse_argv(
    const gchar* cmdline,
    GError**     out_error
)
{
    if (out_error != NULL) { *out_error = NULL; }

    gchar** argv;
    GError* error = NULL;

    // Parse cmdline into argv
    //
    g_shell_parse_argv(
        cmdline,
        NULL,
        &argv,
        &error
    );

    if (error != NULL)
    {
        if (out_error != NULL) { *out_error = g_error_copy(error); }
        
        g_clear_error(&error);

        return NULL;
    }

    // Resolve path for executable (we might only have the name)
    //
    gchar* tmp = argv[0];

    argv[0] = g_find_program_in_path(tmp);

    g_free(tmp);

    return argv;
}
