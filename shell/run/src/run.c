#include <glib.h>
#include <gtk/gtk.h>

//
// FORWARD DECLARATIONS
//
static void on_window_destroyed(
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
        G_CALLBACK(on_window_destroyed),
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

    // Create buttons
    //
    button_browse = gtk_button_new_with_label("Browse...");
    button_cancel = gtk_button_new_with_label("Cancel");
    button_ok     = gtk_button_new_with_label("OK");

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

    // Launch now
    //
    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

//
// CALLBACKS
//
static void on_window_destroyed(
    GtkWidget* widget,
    gpointer   user_data
)
{
    gtk_main_quit();
}
