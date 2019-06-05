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
#ifndef _gtkinspector_PROPLIST_H_
#define _gtkinspector_PROPLIST_H_


#include <gtk/gtk.h>

#define GTKINSPECTOR_TYPE_PROPLIST            (gtkinspector_proplist_get_type())
#define GTKINSPECTOR_PROPLIST(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GTKINSPECTOR_TYPE_PROPLIST, GtkinspectorPropList))
#define GTKINSPECTOR_PROPLIST_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GTKINSPECTOR_TYPE_PROPLIST, GtkinspectorPropListClass))
#define GTKINSPECTOR_IS_PROPLIST(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTKINSPECTOR_TYPE_PROPLIST))
#define GTKINSPECTOR_IS_PROPLIST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTKINSPECTOR_TYPE_PROPLIST))
#define GTKINSPECTOR_PROPLIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTKINSPECTOR_TYPE_PROPLIST, GtkinspectorPropListClass))


typedef struct _GtkinspectorPropListPrivate GtkinspectorPropListPrivate;

typedef struct _GtkinspectorPropList {
   GtkTreeView parent;
   GtkinspectorPropListPrivate *priv;
} GtkinspectorPropList;

typedef struct _GtkinspectorPropListClass {
   GtkTreeViewClass parent;
} GtkinspectorPropListClass;


G_BEGIN_DECLS

GType      gtkinspector_proplist_get_type   (void);
GtkWidget *gtkinspector_proplist_new        (GtkWidget *widget_tree);
void       gtkinspector_proplist_set_object (GtkinspectorPropList *proplist,
                                         GObject          *object);

G_END_DECLS

#endif // _gtkinspector_PROPLIST_H_

// vim: set et sw=4 ts=4:
