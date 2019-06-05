/*
 * Copyright (c) 2013 Intel Corporation
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

#ifndef _gtkinspector_OBJECTHIERARCHY_H_
#define _gtkinspector_OBJECTHIERARCHY_H_

#include <gtk/gtk.h>

#define GTKINSPECTOR_TYPE_OBJECTHIERARCHY            (gtkinspector_objecthierarchy_get_type())
#define GTKINSPECTOR_OBJECTHIERARCHY(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GTKINSPECTOR_TYPE_OBJECTHIERARCHY, GtkinspectorObjectHierarchy))
#define GTKINSPECTOR_OBJECTHIERARCHY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GTKINSPECTOR_TYPE_OBJECTHIERARCHY, GtkinspectorObjectHierarchyClass))
#define GTKINSPECTOR_IS_OBJECTHIERARCHY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTKINSPECTOR_TYPE_OBJECTHIERARCHY))
#define GTKINSPECTOR_IS_OBJECTHIERARCHY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTKINSPECTOR_TYPE_OBJECTHIERARCHY))
#define GTKINSPECTOR_OBJECTHIERARCHY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTKINSPECTOR_TYPE_OBJECTHIERARCHY, GtkinspectorObjectHierarchyClass))


typedef struct _GtkinspectorObjectHierarchyPrivate GtkinspectorObjectHierarchyPrivate;

typedef struct _GtkinspectorObjectHierarchy {
   GtkBox parent;
   GtkinspectorObjectHierarchyPrivate *priv;
} GtkinspectorObjectHierarchy;

typedef struct _GtkinspectorObjectHierarchyClass {
   GtkBoxClass parent;
} GtkinspectorObjectHierarchyClass;

G_BEGIN_DECLS

GType     gtkinspector_objecthierarchy_get_type   (void);
GtkWidget *gtkinspector_objecthierarchy_new       (void);
void      gtkinspector_objecthierarchy_set_object (GtkinspectorObjectHierarchy *editor,
                                               GObject                 *object);

G_END_DECLS

#endif // _gtkinspector_OBJECTHIERARCHY_H_

// vim: set et sw=4 ts=4:
