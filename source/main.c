#include "../include/alxvmrexample.h"

static GMainLoop *loop = NULL;
#define SERVICE_PATH "/org/alxvmr/example"
#define SERVICE_NAME "org.alxvmr.example"

static void method_call_cb (GDBusConnection       *connection,
                            const gchar           *sender,
                            const gchar           *object_path,
                            const gchar           *interface_name,
                            const gchar           *method_name,
                            GVariant              *parameters,
                            GDBusMethodInvocation *invocation,
                            gpointer               user_data)
{
    AlxvmrExample *example = ALXVMR_EXAMPLE(user_data);
    GError *error = NULL;

    if (g_strcmp0(method_name, "setvalue1") == 0) {
        gint new_value;
        g_variant_get(parameters, "(i)", &new_value);
        
        if (alxvmr_example_setvalue1 (example, new_value, &error)) {
            g_dbus_method_invocation_return_value(invocation, NULL);
        }
        else {
            g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_FAILED, "Failed to set value1: %s", error->message);
            g_error_free(error);
        }
        return;
    }

    if (g_strcmp0(method_name, "getvalue1") == 0) {
        gint new_value;
        g_variant_get(parameters, "(i)", &new_value);

        if (alxvmr_example_getvalue1 (example, new_value, &error)) {
            g_dbus_method_invocation_return_value(invocation, NULL);
        }
        else {
            g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_FAILED, "Failed to get value1: %s", error->message);
            g_error_free(error);
        }
        return;
    }
}

static const GDBusInterfaceVTable interface_vtable =
{
    .method_call = method_call_cb,
    .get_property = NULL,
    .set_property = NULL
};


int main (int argc, char *argv[])
{
    DBusGConnection *bus = NULL;
    AlxvmrExample *obj = alxvmr_example_new();
    GError *error = NULL;
    guint registration_id;
    GBusNameOwnerFlags request_name_result;

    bus = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (bus == NULL) {
        g_critical("error getting session bus: %s", error->message);
    }

    registration_id = g_dbus_connection_register_object(bus,
                                                        SERVICE_PATH,
                                                        alxvmr_example_get_introspection_data(obj)->interfaces[0],
                                                        &interface_vtable,
                                                        g_object_ref(obj),
                                                        g_object_unref,
                                                        error);

    g_assert (registration_id > 0);

    request_name_result = g_bus_own_name(G_BUS_TYPE_SESSION,
                                         SERVICE_NAME,
                                         G_BUS_NAME_OWNER_FLAGS_NONE,
                                         NULL,
                                         NULL,
                                         NULL,
                                         bus,
                                         NULL);

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);
    g_bus_unown_name (request_name_result);
    
    return 0;
}
