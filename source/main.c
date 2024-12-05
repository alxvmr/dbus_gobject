#include "../include/alxvmrexample.h"

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
    AlxvmrExample *self = (AlxvmrExample *) user_data;
    GError *error = NULL;

    if (g_strcmp0(method_name, "setvalue1") == 0) {
        gint new_value;
        g_variant_get(parameters, "(i)", &new_value);
        
        if (alxvmr_example_setvalue1 (self, new_value, &error)) {
            g_dbus_method_invocation_return_value(invocation, NULL);
        }
        else {
            g_dbus_method_invocation_return_error(invocation, G_DBUS_ERROR, G_DBUS_ERROR_FAILED, "Failed to set value1: %s", error->message);
            g_error_free(error);
        }
        return;
    }

    if (g_strcmp0(method_name, "getvalue1") == 0) {
        gint out_value;
        if (alxvmr_example_getvalue1 (self, &out_value, &error)) {
            g_dbus_method_invocation_return_value(invocation, g_variant_new("(i)", out_value));
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

static void on_bus_acquired (GDBusConnection *bus, 
                             const gchar *name,
                             gpointer user_data)
{
    GError *error = NULL;
    guint registration_id;
    AlxvmrExample *self = (AlxvmrExample *) user_data;
    g_assert(self != NULL);

    registration_id = g_dbus_connection_register_object(bus,
                                                        SERVICE_PATH,
                                                        alxvmr_example_get_introspection_data(self)->interfaces[0],
                                                        &interface_vtable,
                                                        g_object_ref(self),
                                                        g_object_unref,
                                                        error);

    if (error) {
        g_critical("Failed to register object: %s", error->message);
        g_error_free(error);
        return;
    }

    g_assert(registration_id > 0);
}


int main (int argc, char *argv[])
{
    DBusGConnection *bus = NULL;
    AlxvmrExample *obj = alxvmr_example_new();
    GError *error = NULL;
    guint registration_id;
    guint owner_id;

    bus = g_bus_get_sync(G_BUS_TYPE_SESSION, NULL, &error);
    if (bus == NULL) {
        g_critical("error getting session bus: %s", error->message);
    }

    owner_id = g_bus_own_name(G_BUS_TYPE_SESSION,
                              SERVICE_NAME,
                              G_BUS_NAME_OWNER_FLAGS_NONE,
                              on_bus_acquired,
                              NULL,
                              NULL,
                              obj,
                              NULL);

    GMainLoop *loop = g_main_loop_new(NULL, FALSE);
    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);
    g_bus_unown_name (owner_id);
    
    return 0;
}
