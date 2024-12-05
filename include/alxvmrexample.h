#ifndef ALXVMREXAMPLE_H
#define ALXVMREXAMPLE_H
#include <glib-object.h>
#include <dbus/dbus-glib.h>
#include <gio/gio.h>
G_BEGIN_DECLS

#define ALXVMR_TYPE_EXAMPLE (alxvmr_example_get_type())
G_DECLARE_FINAL_TYPE (AlxvmrExample, alxvmr_example, ALXVMR, EXAMPLE, GObject)

typedef struct _AlxvmrExample AlxvmrExample;

AlxvmrExample* alxvmr_example_new(void);
GDBusNodeInfo* alxvmr_example_get_introspection_data(AlxvmrExample* self);

gboolean alxvmr_example_setvalue1(AlxvmrExample *obj, gint valueIn, GError **error);
gboolean alxvmr_example_getvalue1 (AlxvmrExample* obj, gint* valueOut, GError **error);
G_END_DECLS

#endif