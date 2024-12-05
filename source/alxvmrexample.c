#include "alxvmrexample.h"

struct _AlxvmrExample {
    GObjectClass parent;
    GDBusNodeInfo *introspection_data;
    gchar* introspection_xml;
    gint value1;
    gdouble value2;
};

G_DEFINE_TYPE(AlxvmrExample, alxvmr_example, G_TYPE_OBJECT)

static const gchar* xml = "<node>"
"  <interface name='org.alxvmr.example'>"
"    <!-- getvalue1(): returns the first value (int) -->"
"    <method name='getvalue1'>"
"      <arg type='i' name='cur_value' direction='out'/>"
"    </method>"
"    <!-- setvalue1(int newValue): sets value1 -->"
"    <method name='setvalue1'>"
"      <arg type='i' name='new_value' direction='in'/>"
"    </method>"
"  </interface>"
"</node>";

static void
alxvmr_example_class_init(AlxvmrExampleClass* self)
{
}

static void
alxvmr_example_init(AlxvmrExample* self)
{
}

GDBusNodeInfo*
alxvmr_example_get_introspection_data(AlxvmrExample* self)
{
    return self->introspection_data;
}

AlxvmrExample*
alxvmr_example_new (void)
{
    AlxvmrExample* self = ALXVMR_EXAMPLE(g_object_new(ALXVMR_TYPE_EXAMPLE, NULL));
    self->value1 = 0;
    self->value2 = 0.0;
    self->introspection_xml = xml;
    self->introspection_data = g_dbus_node_info_new_for_xml(xml, NULL);
    return self;
}

gboolean alxvmr_example_setvalue1 (AlxvmrExample* obj, gint valueIn, GError** error)
{
    dbg ("Called (valueIn) = %d", valueIn);
    g_assert (obj != NULL);
    obj->value1 = valueIn;

    return TRUE;
}

gboolean alxvmr_example_getvalue1 (AlxvmrExample* obj, gint* valueOut, GError** error)
{
    dbg ("Called (internal value1 is %d)", obj->value1);

    g_assert(obj != NULL);
    g_assert(valueOut != NULL);

    *valueOut = obj->value1;

    return TRUE;
}
