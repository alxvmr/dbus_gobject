#include "../include/passwdservice.h"
#include "../include/passwduser.h"

#define SERVICE_PATH "/org/alxvmr/passwd"
#define SERVICE_NAME "org.alxvmr.passwd"

static void
method_call_cb (GDBusConnection       *connection,
                const gchar           *sender,
                const gchar           *object_path,
                const gchar           *interface_name,
                const gchar           *method_name,
                GVariant              *parameters,
                GDBusMethodInvocation *invocation,
                gpointer               user_data)
{
    PasswdService *self = (PasswdService *) user_data;
    GError *error = NULL;

    g_assert (self != NULL);

    if (g_strcmp0 (method_name, "setPassword") == 0) {
        gchar *output = NULL;
        gchar *user_name = NULL;
        gchar *old_passwd = NULL;
        gchar *new_passwd = NULL;

        g_variant_get (parameters, "(sss)", 
                       &user_name, &old_passwd, &new_passwd);
        PasswdUser *user = passwd_user_new (user_name, old_passwd, new_passwd);

        if (passwd_service_set_password (self,
                                         user,
                                         output, 
                                         &error))
        {
            g_dbus_method_invocation_return_value (invocation, g_variant_new (("s"), output));
        }
        else {
            g_dbus_method_invocation_return_error(invocation, error->domain, error->code, "%s", error->message);
            g_error_free(error);
        }
        return;
    }
}

static const
GDBusInterfaceVTable interface_vtable =
{
    .method_call = method_call_cb,
    .get_property = NULL,
    .set_property = NULL
};

static void
on_bus_acquired (GDBusConnection *bus,
                 const gchar     *name,
                 gpointer         user_data)
{
    GError *error = NULL;
    guint registration_id;

    PasswdService *self = (PasswdService *) user_data;
    g_assert (self != NULL);

    registration_id = g_dbus_connection_register_object (bus,
                                                         SERVICE_PATH,
                                                         passwd_service_get_introspection_data(self)->interfaces[0],
                                                         &interface_vtable,
                                                         g_object_ref(self),
                                                         NULL,
                                                         &error);

    if (error) {
        g_critical ("Failed to register object: %s", error->message);
        g_error_free (error);
        return;
    }

    g_assert (registration_id > 0);
}

int main (int argc, char *argv[])
{
    DBusGConnection *bus = NULL;
    GError *error = NULL;
    GMainLoop *loop = NULL;
    PasswdService *passwdservice = passwd_service_new();
    guint owner_id;

    bus = dbus_g_bus_get (G_BUS_TYPE_SESSION, &error);

    if (error != NULL || bus == NULL) {
        g_critical ("Error getting session bus: %s", error->message);
        g_error_free (error);
        return 1;
    }

    owner_id = g_bus_own_name (G_BUS_TYPE_SESSION,
                               SERVICE_NAME,
                               G_BUS_NAME_OWNER_FLAGS_NONE,
                               on_bus_acquired,
                               NULL,
                               NULL,
                               passwdservice,
                               NULL);

    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);

    g_main_loop_unref (loop);
    g_bus_unown_name (owner_id);
    dbus_g_connection_unref (bus);

    return 0;
}