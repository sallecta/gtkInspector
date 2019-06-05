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
#include "prop-list.h"
#include "property-cell-renderer.h"


enum
{
  COLUMN_NAME,
  COLUMN_VALUE,
  COLUMN_DEFINED_AT,
  COLUMN_OBJECT,
  COLUMN_TOOLTIP,
  COLUMN_RO,
  NUM_COLUMNS
};

enum
{
  PROP_0,
  PROP_WIDGET_TREE
};

struct _GtkinspectorPropListPrivate
{
  GObject *object;
  GtkListStore *model;
  GHashTable *prop_iters;
  GList *signal_cnxs;
  GtkWidget *widget_tree;
  GtkTreeViewColumn *property_column;
};

G_DEFINE_TYPE_WITH_PRIVATE (GtkinspectorPropList, gtkinspector_proplist, GTK_TYPE_TREE_VIEW)

static void
gtkinspector_proplist_init (GtkinspectorPropList *pl)
{
  pl->priv = gtkinspector_proplist_get_instance_private (pl);
}

static gboolean
query_tooltip_cb (GtkWidget        *widget,
			      gint             x,
			      gint             y,
			      gboolean         keyboard_tip,
			      GtkTooltip       *tooltip,
			      GtkinspectorPropList *pl)
{
  GtkTreeIter iter;
  GtkTreeView *tree_view = GTK_TREE_VIEW (widget);
  GtkTreeModel *model = gtk_tree_view_get_model (tree_view);
  GtkTreePath *path = NULL;
  gchar *tooltip_text;

  if (!gtk_tree_view_get_tooltip_context (tree_view,
                                          &x,
                                          &y,
                                          keyboard_tip,
                                          &model,
                                          &path,
                                          &iter))
    return FALSE;

  gtk_tree_model_get (model, &iter, COLUMN_TOOLTIP, &tooltip_text, -1);
  gtk_tooltip_set_text (tooltip, tooltip_text);

  gtk_tree_view_set_tooltip_cell (tree_view,
                                  tooltip,
                                  path,
                                  pl->priv->property_column,
                                  NULL);

  gtk_tree_path_free (path);
  g_free (tooltip_text);

  return TRUE;
}

static void
draw_columns (GtkTreeViewColumn *column,
              GtkCellRenderer   *renderer,
              GtkTreeModel      *model,
              GtkTreeIter       *iter,
              GtkinspectorPropList  *pl)
{
  gboolean ro;

  gtk_tree_model_get (model, iter, COLUMN_RO, &ro, -1);
  if (ro)
    {
      g_object_set (renderer, "foreground", "#a7aba7", NULL);
    }
  else
    {
      g_object_set (renderer, "foreground-set", FALSE, NULL);
    }
}

static void
constructed (GObject *object)
{
  GtkinspectorPropList *pl = GTKINSPECTOR_PROPLIST (object);
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  pl->priv->prop_iters = g_hash_table_new_full (g_str_hash,
                                                g_str_equal,
                                                NULL,
                                                (GDestroyNotify) gtk_tree_iter_free);

  pl->priv->model = gtk_list_store_new(NUM_COLUMNS,
                                       G_TYPE_STRING,  // COLUMN_NAME
                                       G_TYPE_STRING,  // COLUMN_VALUE
                                       G_TYPE_STRING,  // COLUMN_DEFINED_AT
                                       G_TYPE_OBJECT,  // COLUMN_OBJECT
                                       G_TYPE_STRING,  // COLUMN_TOOLTIP
                                       G_TYPE_BOOLEAN);// COLUMN_RO
  gtk_tree_view_set_model (GTK_TREE_VIEW (pl),
                           GTK_TREE_MODEL (pl->priv->model));

  renderer = gtk_cell_renderer_text_new();
  g_object_set (renderer, "scale", TREE_TEXT_SCALE, NULL);
  pl->priv->property_column = gtk_tree_view_column_new_with_attributes ("Property",
                                                                        renderer,
                                                                        "text", COLUMN_NAME,
                                                                        NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (pl), pl->priv->property_column);
  g_object_set (pl->priv->property_column,
                "resizable", TRUE,
                "sort-order", GTK_SORT_ASCENDING,
                "sort-column-id", COLUMN_NAME,
                NULL);
 gtk_tree_view_column_set_cell_data_func (pl->priv->property_column,
                                          renderer,
                                          (GtkTreeCellDataFunc) draw_columns,
                                          pl,
                                          NULL);

  renderer = gtkinspector_property_cell_renderer_new ();
  g_object_set_data (G_OBJECT (renderer), "gtkinspector-widget-tree", pl->priv->widget_tree);
  g_object_set (renderer,
                "scale", TREE_TEXT_SCALE,
                "editable", TRUE,
                NULL);
  column = gtk_tree_view_column_new_with_attributes ("Value", renderer,
                                                     "text", COLUMN_VALUE,
                                                     "object", COLUMN_OBJECT,
                                                     "name", COLUMN_NAME,
                                                      NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (pl), column);
  gtk_tree_view_column_set_resizable (column, TRUE);

  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (pl->priv->model),
                                        COLUMN_NAME,
                                        GTK_SORT_ASCENDING);
 gtk_tree_view_column_set_cell_data_func (column,
                                          renderer,
                                          (GtkTreeCellDataFunc) draw_columns,
                                          pl,
                                          NULL);

  renderer = gtk_cell_renderer_text_new ();
  g_object_set (renderer, "scale", TREE_TEXT_SCALE, NULL);
  column = gtk_tree_view_column_new_with_attributes ("Defined at",
                                                     renderer,
                                                     "text", COLUMN_DEFINED_AT,
                                                     NULL);
  gtk_tree_view_append_column (GTK_TREE_VIEW (pl), column);
  gtk_tree_view_column_set_cell_data_func (column,
                                           renderer,
                                           (GtkTreeCellDataFunc) draw_columns,
                                           pl,
                                           NULL);

  g_object_set (object, "has-tooltip", TRUE, NULL);
  g_signal_connect (object, "query-tooltip", G_CALLBACK (query_tooltip_cb), pl);
}

static void
get_property (GObject    *object,
              guint      param_id,
              GValue     *value,
              GParamSpec *pspec)
{
  GtkinspectorPropList *pl = GTKINSPECTOR_PROPLIST (object);

  switch (param_id)
    {
      case PROP_WIDGET_TREE:
        g_value_take_object (value, pl->priv->widget_tree);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
        break;
    }
}

static void
set_property (GObject      *object,
              guint        param_id,
              const GValue *value,
              GParamSpec   *pspec)
{
  GtkinspectorPropList *pl = GTKINSPECTOR_PROPLIST (object);

  switch (param_id)
    {
      case PROP_WIDGET_TREE:
        pl->priv->widget_tree = g_value_get_object (value);
        break;

      default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, param_id, pspec);
        break;
    }
}

static void
gtkinspector_proplist_class_init (GtkinspectorPropListClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS(klass);

  object_class->get_property = get_property;
  object_class->set_property = set_property;
  object_class->constructed  = constructed;

  g_object_class_install_property (object_class,
                                   PROP_WIDGET_TREE,
                                   g_param_spec_object ("widget-tree",
                                                         "Widget Tree",
                                                         "Widget tree",
                                                         GTK_TYPE_WIDGET,
                                                         G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void
gtkinspector_prop_list_update_prop (GtkinspectorPropList *pl,
                                GtkTreeIter *iter,
                                GParamSpec *prop)
{
  GValue gvalue = {0};
  char *value;

  g_value_init(&gvalue, prop->value_type);
  g_object_get_property (pl->priv->object, prop->name, &gvalue);

  if (G_VALUE_HOLDS_ENUM (&gvalue))
    {
      GEnumClass *enum_class = G_PARAM_SPEC_ENUM(prop)->enum_class;
      GEnumValue *enum_value = g_enum_get_value(enum_class, g_value_get_enum(&gvalue));

      value = g_strdup (enum_value->value_name);
    }
  else
    {
      value = g_strdup_value_contents(&gvalue);
    }

  gtk_list_store_set (pl->priv->model, iter,
                      COLUMN_NAME, prop->name,
                      COLUMN_VALUE, value,
                      COLUMN_DEFINED_AT, g_type_name (prop->owner_type),
                      COLUMN_OBJECT, pl->priv->object,
                      COLUMN_TOOLTIP, g_param_spec_get_blurb (prop),
                      COLUMN_RO, !(prop->flags & G_PARAM_WRITABLE),
                      -1);

  g_free (value);
  g_value_unset (&gvalue);
}

static void
gtkinspector_proplist_prop_changed_cb (GObject *pspec,
                                   GParamSpec *prop,
                                   GtkinspectorPropList *pl)
{
  GtkTreeIter *iter = g_hash_table_lookup(pl->priv->prop_iters, prop->name);

  if (iter != NULL)
    gtkinspector_prop_list_update_prop (pl, iter, prop);
}

GtkWidget *
gtkinspector_proplist_new (GtkWidget *widget_tree)
{
    return g_object_new (GTKINSPECTOR_TYPE_PROPLIST,
                         "widget-tree", widget_tree,
                         NULL);
}

void
gtkinspector_proplist_set_object (GtkinspectorPropList* pl, GObject *object)
{
  GtkTreeIter iter;
  GParamSpec **props;
  guint num_properties;
  guint i;
  GList *l;

  pl->priv->object = object;

  for (l = pl->priv->signal_cnxs; l != NULL; l = l->next)
    {
      gulong id = GPOINTER_TO_UINT (l->data);

      if (g_signal_handler_is_connected (object, id))
        g_signal_handler_disconnect (object, id);
    }

  g_list_free (pl->priv->signal_cnxs);
  pl->priv->signal_cnxs = NULL;

  g_hash_table_remove_all (pl->priv->prop_iters);
  gtk_list_store_clear (pl->priv->model);

  props = g_object_class_list_properties (G_OBJECT_GET_CLASS (object), &num_properties);
  for (i = 0; i < num_properties; i++)
    {
      GParamSpec *prop = props[i];
      char *signal_name;

      if (! (prop->flags & G_PARAM_READABLE))
        continue;

      gtk_list_store_append (pl->priv->model, &iter);
      gtkinspector_prop_list_update_prop (pl, &iter, prop);

      g_hash_table_insert (pl->priv->prop_iters, (gpointer) prop->name, gtk_tree_iter_copy (&iter));

      /* Listen for updates */
      signal_name = g_strdup_printf ("notify::%s", prop->name);

      pl->priv->signal_cnxs =
            g_list_prepend (pl->priv->signal_cnxs, GINT_TO_POINTER(
                g_signal_connect(object, signal_name,
                                 G_CALLBACK (gtkinspector_proplist_prop_changed_cb),
                                 pl)));

        g_free (signal_name);
    }
}


// vim: set et sw=4 ts=4:
