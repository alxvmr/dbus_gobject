#include <pwd.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/passwdservice.h"
#include "../include/passwd_pam.h"

G_DEFINE_TYPE (PasswdService, passwd_service, G_TYPE_OBJECT)

static const gchar *xml = "<node>"
"  <interface name='org.passwd.service'>"
"    <!-- setPassword(): change user's password -->"
"    <method name='setPassword'>"
"      <arg type='s' name='user_name' direction='in'/>"
"      <arg type='s' name='old_passwd' direction='in'/>"
"      <arg type='s' name='new_passwd' direction='in'/>"
"      <arg type='s' name='output' direction='out'/>"
"    </method>"
"  </interface>"
"</node>";

static void
passwd_service_class_init (PasswdServiceClass *self)
{}

static void
passwd_service_init (PasswdService *self)
{}

GDBusNodeInfo*
passwd_service_get_introspection_data (PasswdService *self)
{
    //g_assert (self->introspection_data != NULL);
    return self->introspection_data;
}

PasswdService*
passwd_service_new (void)
{
    PasswdService *self = PASSWD_SERVICE (g_object_new (PASSWD_TYPE_SERVICE, NULL));
    self->introspection_data = g_dbus_node_info_new_for_xml(xml, NULL);

    return self;
}

gboolean
passwd_service_set_password (PasswdService  *self,
                             PasswdUser     *user,
                             gchar          *output,
                             GError         **error)
{
    g_assert (self != NULL);
    g_assert (user != NULL);

    int retval = setup_pam (user, error);

    if (error != NULL) {
        return FALSE;
    }

    return TRUE;
}