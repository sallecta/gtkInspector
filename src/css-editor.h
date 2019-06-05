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

#ifndef _gtkinspector_CSSEDITOR_H_
#define _gtkinspector_CSSEDITOR_H_

#include <gtk/gtk.h>

#define GTKINSPECTOR_TYPE_CSSEDITOR            (gtkinspector_csseditor_get_type())
#define GTKINSPECTOR_CSSEDITOR(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GTKINSPECTOR_TYPE_CSSEDITOR, GtkinspectorCssEditor))
#define GTKINSPECTOR_CSSEDITOR_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GTKINSPECTOR_TYPE_CSSEDITOR, GtkinspectorCssEditorClass))
#define GTKINSPECTOR_IS_CSSEDITOR(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTKINSPECTOR_TYPE_CSSEDITOR))
#define GTKINSPECTOR_IS_CSSEDITOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTKINSPECTOR_TYPE_CSSEDITOR))
#define GTKINSPECTOR_CSSEDITOR_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTKINSPECTOR_TYPE_CSSEDITOR, GtkinspectorCssEditorClass))


typedef struct _GtkinspectorCssEditorPrivate GtkinspectorCssEditorPrivate;

typedef struct _GtkinspectorCssEditor {
   GtkBox parent;
   GtkinspectorCssEditorPrivate *priv;
} GtkinspectorCssEditor;

typedef struct _GtkinspectorCssEditorClass {
   GtkBoxClass parent;
} GtkinspectorCssEditorClass;

G_BEGIN_DECLS

GType gtkinspector_csseditor_get_type ();

GtkWidget *gtkinspector_csseditor_new (gboolean global);
void gtkinspector_csseditor_set_widget (GtkinspectorCssEditor *editor,
                                    GtkWidget         *widget);

G_END_DECLS

#endif // _gtkinspector_CSSEDITOR_H_

// vim: set et sw=4 ts=4:
