#ifndef PASSWDSERVICE_H
#define PASSWDSERVICE_H
#include <glib-object.h>
#include <dbus/dbus-glib.h>
#include <gio/gio.h>

#define dbg(fmtstr, args...) \
  (g_print(PROGNAME ":%s: " fmtstr "\n", __func__, ##args))

G_BEGIN_DECLS

#define PASSWD_TYPE_SERVICE (passwd_service_get_type())
G_DECLARE_FINAL_TYPE (PasswdService, passwd_service, PASSWD, SERVICE, GObject)

struct _PasswdService {
    GObjectClass parent;
    GDBusNodeInfo *introspection_data;
    gchar *user_name;
    gchar *old_passwd;
    gchar *new_passwd;
};

typedef struct _PasswdService PasswdService;

PasswdService *passwd_service_new (void);
GDBusNodeInfo *passwd_service_get_introspection_data (PasswdService *self);

gboolean passwd_service_set_password (PasswdService   *self,
                                      gchar           *output,
                                      gchar           *user_name,
                                      gchar           *old_pwd,
                                      gchar           *new_pwd,
                                      GError          **error);

gchar ** passwd_service_get_fields (PasswdService *self);
G_END_DECLS

#endif