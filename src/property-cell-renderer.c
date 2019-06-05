/*
 * Copyright (c) 2008-2009  Christian Hammond
 * Copyright (c) 2008-2009  David Trowbridge
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#include "gtkinspector.h"
#include "property-cell-renderer.h"
#include "widget-tree.h"

struct _GtkinspectorPropertyCellRendererPrivate
{
  GObject *object;
  char *name;
};

enum
{
  PROP_0,
  PROP_OBJECT,
  PROP_NAME
};

G_DEFINE_TYPE_WITH_PRIVATE (GtkinspectorPropertyCellRenderer, gtkinspector_property_cell_renderer, GTK_TYPE_CELL_RENDERER_TEXT);

static void
gtkinspector_property_cell_renderer_init(GtkinspectorPropertyCellRenderer *renderer)
{
  renderer->priv = gtkinspector_property_cell_renderer_get_instance_private (renderer);
}

static void
get_property (GObject *object,
              guint param_id,
              GValue *value,
              GParamSpec *pspec)
{
  GtkinspectorPropertyCellRenderer *r = GTKINSPECTOR_PROPERTY_CELL_RENDERER (object);

  switch (param_id)
    {
      case PROP_OBJECT:
        g_value_set_object(value, r->priv->object);
        break;

      case PROP_NAME:
        g_value_set_string(value, r->priv->name);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
        break;
    }
}

static void
set_property (GObject *object,
              guint param_id,
              const GValue *value,
              GParamSpec *pspec)
{
  GtkinspectorPropertyCellRenderer *r = GTKINSPECTOR_PROPERTY_CELL_RENDERER (object);

  switch (param_id)
    {
      case PROP_OBJECT:
        r->priv->object = g_value_get_object (value);
        break;

      case PROP_NAME:
        g_free (r->priv->name);
        r->priv->name = g_value_dup_string (value);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
        break;
    }
}

static void
stop_editing(GtkCellEditable *editable, GtkCellRenderer *renderer)
{
    GObject *object;
    const char *name;
    GValue gvalue = {0};
    GParamSpec *prop;

    object = g_object_get_data(G_OBJECT(editable), "_prop_object");
    name   = g_object_get_data(G_OBJECT(editable), "_prop_name");

    prop = g_object_class_find_property(G_OBJECT_GET_CLASS(object), name);
    g_value_init(&gvalue, prop->value_type);

    if (GTK_IS_ENTRY(editable))
    {
        gboolean canceled;
        g_object_get(editable, "editing_canceled", &canceled, NULL);
        gtk_cell_renderer_stop_editing(renderer, canceled);

        if (canceled)
            return;

        if (GTK_IS_SPIN_BUTTON(editable))
        {
            double value =
                g_ascii_strtod(gtk_entry_get_text(GTK_ENTRY(editable)), NULL);

            if (G_IS_PARAM_SPEC_INT(prop))
                g_value_set_int(&gvalue, (gint)value);
            else if G_IS_PARAM_SPEC_UINT(prop)
                g_value_set_uint(&gvalue, (guint)value);
            else if G_IS_PARAM_SPEC_INT64(prop)
                g_value_set_int64(&gvalue, (gint64)value);
            else if G_IS_PARAM_SPEC_UINT64(prop)
                g_value_set_uint64(&gvalue, (guint64)value);
            else if G_IS_PARAM_SPEC_LONG(prop)
                g_value_set_long(&gvalue, (glong)value);
            else if G_IS_PARAM_SPEC_ULONG(prop)
                g_value_set_ulong(&gvalue, (gulong)value);
            else if G_IS_PARAM_SPEC_DOUBLE(prop)
                g_value_set_double(&gvalue, (gdouble)value);
            else
                return;
        }
        else
        {
            g_value_set_string(&gvalue,
                               gtk_entry_get_text(GTK_ENTRY(editable)));
        }
    }
    else if (GTK_IS_COMBO_BOX(editable))
    {
        // We have no way of getting the canceled state for a GtkComboBox.
        gtk_cell_renderer_stop_editing(renderer, FALSE);

        if (G_IS_PARAM_SPEC_BOOLEAN(prop))
        {
            g_value_set_boolean(&gvalue,
                gtk_combo_box_get_active(GTK_COMBO_BOX(editable)) == 1);
        }
        else if (G_IS_PARAM_SPEC_ENUM(prop))
        {
            char *enum_name = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (editable));
            GEnumClass *enum_class;
            GEnumValue *enum_value;

            if (enum_name == NULL)
                return;

            enum_class = G_PARAM_SPEC_ENUM(prop)->enum_class;
            enum_value = g_enum_get_value_by_name(enum_class, enum_name);
            g_value_set_enum(&gvalue, enum_value->value);

            g_free(enum_name);
        }
    }

    g_object_set_property(object, name, &gvalue);
    g_value_unset(&gvalue);
}

static GtkCellEditable *
start_editing (GtkCellRenderer *renderer,
               GdkEvent *event,
               GtkWidget *widget,
               const gchar *path,
               const GdkRectangle *background_area,
               const GdkRectangle *cell_area,
               GtkCellRendererState flags)
{
    PangoFontDescription *font_desc;
    GtkCellEditable *editable = NULL;
    GObject *object;
    const char *name;
    GValue gvalue = {0};
    GParamSpec *prop;

    g_object_get(renderer,
                 "object", &object,
                 "name", &name,
                 NULL);

    prop = g_object_class_find_property(G_OBJECT_GET_CLASS(object), name);
    g_value_init(&gvalue, prop->value_type);
    g_object_get_property(object, name, &gvalue);

    if (G_VALUE_HOLDS_OBJECT (&gvalue))
      {
        GtkinspectorWidgetTree *widget_tree = g_object_get_data (G_OBJECT (renderer), "gtkinspector-widget-tree");
        GObject *prop_object = g_value_get_object (&gvalue);
        GtkTreeIter iter;

        if (prop_object)
          {
            /* First check if the value is already in the tree (happens with 'parent' for instance) */
            if (gtkinspector_widget_tree_find_object (widget_tree, prop_object, &iter))
              {
                gtkinspector_widget_tree_select_object (widget_tree, prop_object);
              }
            else if (gtkinspector_widget_tree_find_object (widget_tree, object, &iter))
              {
                gtkinspector_widget_tree_append_object (widget_tree, prop_object, &iter);
                gtkinspector_widget_tree_select_object (widget_tree, prop_object);
              }
            else
              {
                 g_warning ("Gtkinspector: couldn't find the widget in the tree");
              }
          }
        g_value_unset (&gvalue);
        return NULL;
      }
    else
      {
        if (!(prop->flags & G_PARAM_WRITABLE))
          return NULL;

     if (G_VALUE_HOLDS_ENUM(&gvalue) || G_VALUE_HOLDS_BOOLEAN(&gvalue))
    {
        GtkWidget *combobox = gtk_combo_box_text_new ();
        gtk_widget_show(combobox);
        g_object_set(G_OBJECT(combobox), "has-frame", FALSE, NULL);
        GList *renderers;

        if (G_VALUE_HOLDS_BOOLEAN(&gvalue))
        {
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox), "FALSE");
            gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox), "TRUE");

            gtk_combo_box_set_active(GTK_COMBO_BOX (combobox), g_value_get_boolean (&gvalue) ? 1 : 0);
        }
        else if (G_VALUE_HOLDS_ENUM(&gvalue))
        {
            gint value = g_value_get_enum(&gvalue);
            GEnumClass *enum_class = G_PARAM_SPEC_ENUM(prop)->enum_class;
            guint i;

            for (i = 0; i < enum_class->n_values; i++)
            {
                GEnumValue *enum_value = &enum_class->values[i];

                gtk_combo_box_text_append_text (GTK_COMBO_BOX_TEXT (combobox),
                                                enum_value->value_name);

                if (enum_value->value == value)
                    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), i);
            }

        }

        renderers = gtk_cell_layout_get_cells(GTK_CELL_LAYOUT(combobox));
        g_object_set(G_OBJECT(renderers->data), "scale", TREE_TEXT_SCALE, NULL);
        g_list_free(renderers);

        editable = GTK_CELL_EDITABLE(combobox);
    }
    else if (G_VALUE_HOLDS_STRING(&gvalue))
    {
        GtkWidget *entry = gtk_entry_new();
        gtk_widget_show(entry);
        gtk_entry_set_text(GTK_ENTRY(entry), g_value_get_string(&gvalue));

        editable = GTK_CELL_EDITABLE(entry);
    }
    else if (G_VALUE_HOLDS_INT(&gvalue)    ||
             G_VALUE_HOLDS_UINT(&gvalue)   ||
             G_VALUE_HOLDS_INT64(&gvalue)  ||
             G_VALUE_HOLDS_UINT64(&gvalue) ||
             G_VALUE_HOLDS_LONG(&gvalue)   ||
             G_VALUE_HOLDS_ULONG(&gvalue)  ||
             G_VALUE_HOLDS_DOUBLE(&gvalue))
    {
        double min, max, value;
        GtkWidget *spinbutton;
        guint digits = 0;

        if (G_VALUE_HOLDS_INT(&gvalue))
        {
            GParamSpecInt *paramspec = G_PARAM_SPEC_INT(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_int(&gvalue);
        }
        else if (G_VALUE_HOLDS_UINT(&gvalue))
        {
            GParamSpecUInt *paramspec = G_PARAM_SPEC_UINT(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_uint(&gvalue);
        }
        else if (G_VALUE_HOLDS_INT64(&gvalue))
        {
            GParamSpecInt64 *paramspec = G_PARAM_SPEC_INT64(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_int64(&gvalue);
        }
        else if (G_VALUE_HOLDS_UINT64(&gvalue))
        {
            GParamSpecUInt64 *paramspec = G_PARAM_SPEC_UINT64(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_uint64(&gvalue);
        }
        else if (G_VALUE_HOLDS_LONG(&gvalue))
        {
            GParamSpecLong *paramspec = G_PARAM_SPEC_LONG(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_long(&gvalue);
        }
        else if (G_VALUE_HOLDS_ULONG(&gvalue))
        {
            GParamSpecULong *paramspec = G_PARAM_SPEC_ULONG(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_ulong(&gvalue);
        }
        else if (G_VALUE_HOLDS_DOUBLE(&gvalue))
        {
            GParamSpecDouble *paramspec = G_PARAM_SPEC_DOUBLE(prop);
            min = paramspec->minimum;
            max = paramspec->maximum;
            value = g_value_get_double(&gvalue);
            digits = 2;
        }
        else
        {
            // Shouldn't really be able to happen.
            return NULL;
        }

        spinbutton = gtk_spin_button_new_with_range(min, max, 1);
        gtk_widget_show(spinbutton);
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinbutton), value);
        gtk_spin_button_set_digits(GTK_SPIN_BUTTON(spinbutton), digits);

        editable = GTK_CELL_EDITABLE(spinbutton);
    }
  }
    g_value_unset(&gvalue);

    if (!editable)
      return NULL;

    font_desc = pango_font_description_new();
    pango_font_description_set_size(font_desc, 8 * PANGO_SCALE);
    gtk_widget_override_font (GTK_WIDGET (editable), font_desc);
    pango_font_description_free(font_desc);

    g_signal_connect(editable, "editing_done", G_CALLBACK (stop_editing), renderer);
    g_object_set_data_full (G_OBJECT (editable), "_prop_name", g_strdup (name), g_free);
    g_object_set_data (G_OBJECT (editable), "_prop_object", object);

    return editable;
}

static void
gtkinspector_property_cell_renderer_class_init (GtkinspectorPropertyCellRendererClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkCellRendererClass *cell_class = GTK_CELL_RENDERER_CLASS (klass);

  object_class->get_property = get_property;
  object_class->set_property = set_property;

  cell_class->start_editing = start_editing;

  g_object_class_install_property(object_class,
        PROP_OBJECT,
        g_param_spec_object ("object",
                             "Object",
                             "The object owning the property",
                             G_TYPE_OBJECT,
                             G_PARAM_READWRITE));

  g_object_class_install_property(object_class,
        PROP_NAME,
        g_param_spec_string ("name",
                             "Name",
                             "The property name",
                             NULL,
                             G_PARAM_READWRITE));
}

GtkCellRenderer *
gtkinspector_property_cell_renderer_new(void)
{
    return g_object_new(GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER, NULL);
}


// vim: set et ts=4:
