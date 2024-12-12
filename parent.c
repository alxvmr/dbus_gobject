#include <gio/gio.h>

int main (int argc, char *argv[]) {
    GError *error = NULL;
    GSubprocess *subprocess;
    GInputStream *input_stream;
    GOutputStream *output_stream;
    GDataInputStream* data_stream ;
    gchar *outline;
    gboolean res = FALSE;

    gchar *input_data = "Hello from parent!";
    data_stream = NULL;
    

    subprocess = g_subprocess_new (G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDIN_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE,
                                   &error,
                                   "./child");
    if (error) {
        g_printerr("Error creating subprocess: %s\n", error->message);
        g_error_free(error);
        g_object_unref(subprocess);
        return 1;
    }

    output_stream = g_subprocess_get_stdin_pipe (subprocess);
    input_stream = g_subprocess_get_stdout_pipe (subprocess);
    data_stream = g_data_input_stream_new(G_INPUT_STREAM(input_stream));

    // send data to child process
    g_output_stream_write (G_OUTPUT_STREAM(output_stream), input_data, strlen(input_data), NULL, &error);
    if (error) {
        g_printerr("Error writing to subprocess: %s\n", error->message);
        g_error_free (error);
        g_object_unref (subprocess);
        return 1;
    }

    // close input stream
    g_output_stream_close (G_OUTPUT_STREAM (output_stream), NULL, &error);
    if (error) {
        g_printerr("Error closing input stream: %s\n", error->message);
        g_error_free (error);
        g_object_unref (subprocess);
        return 1;
    }

    //read data from child process
    do {
        outline = g_data_input_stream_read_line (data_stream, NULL, NULL, &error);
        if (error) {
            g_printerr("Error closing output stream: %s\n", error->message);
            g_error_free (error);
            g_object_unref (subprocess);
            return 1;
        }
        if (outline == NULL) {
            continue;
        }

        g_print("MAIN: %s\n", outline);
        g_free(outline);
    } while (outline);

    res = g_subprocess_wait_check(subprocess, NULL, &error);
    if (error) {
        g_printerr("Error while wait check: %s\n", error->message);
        g_error_free (error);
        g_object_unref (subprocess);
        return 1;
    }

    g_object_unref(subprocess);

    return 0;
}