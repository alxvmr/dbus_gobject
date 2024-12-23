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
                             GError         **error)
{
    g_assert (self != NULL);
    g_assert (user != NULL);

    GSubprocess *subprocess = NULL;
    GError *error_sub = NULL;
    gboolean res = FALSE;

    subprocess = g_subprocess_new (G_SUBPROCESS_FLAGS_STDERR_PIPE,
                                   &error_sub,
                                   "./pam_helper",
                                   user->user_name,
                                   user->old_passwd,
                                   user->new_passwd,
                                   NULL);

    if (error_sub) {
        g_set_error_literal (error, error_sub->domain, error_sub->code, error_sub->message);
        g_error_free (error_sub);
        return FALSE;
    }
    
    res = g_subprocess_wait_check (subprocess, NULL, &error_sub);

    if (res) {
        GBytes *stderr_bytes = NULL;
        GString *error_msg = NULL;
        const gchar *stderr_data = NULL;
        gsize stderr_size;
        GInputStream *stderr_stream = NULL;

        stderr_stream = g_subprocess_get_stderr_pipe (subprocess);
        stderr_bytes = g_input_stream_read_bytes (stderr_stream, 4096, NULL, NULL);
        stderr_data = g_bytes_get_data (stderr_bytes, &stderr_size);

        g_set_error_literal (error, G_IO_ERROR, G_IO_ERROR_FAILED, stderr_data);

        if (error_sub != NULL) {
            g_error_free (error_sub);

        }
        g_object_unref (subprocess);

        return FALSE;
    }

    return TRUE;
}