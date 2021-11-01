#include <gdk/gdk.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <wintc-exec.h>

//
// FORWARD DECLARATIONS
//
static void gtk_widget_add_css(
    GtkWidget*   widget,
    const gchar* css
);

static void on_ok_button_clicked(
    GtkWidget* button,
    gpointer   user_data
);

static void on_quit_event(
    GtkWidget* widget,
    gpointer   user_data
);

//
// ENTRY POINT
//
int main(
    int   argc,
    char* argv[]
)
{
    GtkWidget* box_buttons;
    GtkWidget* box_input;
    GtkWidget* box_instructions;
    GtkWidget* box_outer;
    GtkWidget* button_browse;
    GtkWidget* button_cancel;
    GtkWidget* button_ok;
    GtkWidget* combo_entry;
    GtkWidget* combo_entry_internal;
    GtkWidget* label_instruction;
    GtkWidget* label_open;
    GtkWidget* icon;
    GtkWidget* window;

    gtk_init(&argc, &argv);

    // Create the window
    //
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_size_request(window, 341, 154);
    gtk_window_set_icon_name(GTK_WINDOW(window), "system-run");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_title(GTK_WINDOW(window), "Run");
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_MENU);

    g_signal_connect(
        window,
        "destroy",
        G_CALLBACK(on_quit_event),
        NULL
    );

    // Create icon
    //
    icon = gtk_image_new_from_icon_name("system-run", GTK_ICON_SIZE_DND);

    // Create instruction label
    //
    // FIXME: Localize
    // 
    label_instruction = gtk_label_new("Type the name of a program, folder, document, or Internet resource, and Windows will open it for you.");

    gtk_label_set_line_wrap(GTK_LABEL(label_instruction), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label_instruction), 48);
    gtk_label_set_xalign(GTK_LABEL(label_instruction), 0.0);

    // Create "Open:" label
    //
    label_open = gtk_label_new("Open:");

    // Create combobox entry
    //
    combo_entry = gtk_combo_box_new_with_entry();
    combo_entry_internal = gtk_bin_get_child(GTK_BIN(combo_entry));

    gtk_entry_set_activates_default(
        GTK_ENTRY(combo_entry_internal),
        TRUE
    );

    // Create buttons
    //
    button_browse = gtk_button_new_with_label("Browse...");
    button_cancel = gtk_button_new_with_label("Cancel");
    button_ok     = gtk_button_new_with_label("OK");

    gtk_widget_set_can_default(button_ok, TRUE);

    g_signal_connect(
        button_cancel,
        "clicked",
        G_CALLBACK(on_quit_event),
        NULL
    );

    g_signal_connect(
        button_ok,
        "clicked",
        G_CALLBACK(on_ok_button_clicked),
        combo_entry_internal
    );

    // Create boxes
    //
    box_outer = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_input = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_instructions = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_container_add(GTK_CONTAINER(window), box_outer);

    gtk_box_pack_start(GTK_BOX(box_instructions), icon, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_instructions), label_instruction, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_outer), box_instructions, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_input), label_open, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_input), combo_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_outer), box_input, FALSE, FALSE, 0);

    gtk_box_pack_end(GTK_BOX(box_buttons), button_browse, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(box_buttons), button_cancel, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(box_buttons), button_ok, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_outer), box_buttons, FALSE, FALSE, 0);

    // Set OK button as default widget
    //
    gtk_window_set_default(GTK_WINDOW(window), button_ok);

    // Apply styles
    //
    gtk_widget_add_css(box_outer, "box { margin: 18px 11px 0px; }");
    gtk_widget_add_css(box_instructions, "box { margin-bottom: 13px; }");
    gtk_widget_add_css(box_input, "box { margin-bottom: 34px; }");

    gtk_widget_add_css(icon, "image { margin-right: 11px; }");
    gtk_widget_add_css(label_open, "label { margin-right: 11px; }");

    // Initial positioning
    //
    GdkDisplay*  default_display = gdk_display_get_default();
    GdkRectangle geometry;
    GdkMonitor*  primary_monitor = gdk_display_get_primary_monitor(default_display);

    gdk_monitor_get_geometry(primary_monitor, &geometry);

    gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_SOUTH_EAST); // Critical!
    gtk_window_move(
        GTK_WINDOW(window),
        0,
        geometry.height
    );

    // Display now!
    //
    gtk_widget_show_all(window);
    gtk_window_present_with_time(GTK_WINDOW(window), GDK_CURRENT_TIME);

    gtk_main();

    return 0;
}

//
// PRIVATE FUNCTIONS
//
static void gtk_widget_add_css(
    GtkWidget*   widget,
    const gchar* css
)
{
    GtkCssProvider*  provider = gtk_css_provider_new();
    GtkStyleContext* style    = gtk_widget_get_style_context(widget);

    gtk_css_provider_load_from_data(
        provider,
        css,
        -1,
        NULL
    );

    gtk_style_context_add_provider(
        style,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    g_object_unref(provider);
}

//
// CALLBACKS
//
static void on_ok_button_clicked(
    GtkWidget* button,
    gpointer   user_data
)
{
    GtkEntry* entry = GTK_ENTRY(user_data);

    if (wintc_launch_command(gtk_entry_get_text(entry), NULL))
    {
        gtk_main_quit();
    }
}

static void on_quit_event(
    GtkWidget* widget,
    gpointer   user_data
)
{
    gtk_main_quit();
}
