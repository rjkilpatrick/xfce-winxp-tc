#include <gtk/gtk.h>

#include "exec.h"

//
// PUBLIC FUNCTIONS
//
gboolean wintc_launch_command(
    gchar** command
)
{
    GtkWidget* test_dialog =
        gtk_message_dialog_new(
            NULL,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "This is a test dialog."
        );

    gtk_widget_show_all(test_dialog);

    return TRUE;
}
