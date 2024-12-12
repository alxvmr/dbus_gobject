#include <gio/gio.h>
#include <glib/gstdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    gchar *input_data;

    // Читаем данные из стандартного ввода
    gchar response[256];
    input_data = fgets(response, sizeof(response), stdin);
    if (input_data) {
        g_print("Received from parent: %s\n", input_data);
        // Отправляем данные обратно
        g_print("Hello from child!\n");
    }

    sleep(5);
    g_print("Wake up");

    return 0;
}