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
#ifndef _gtkinspector_PROPERTY_CELL_RENDERER_H_
#define _gtkinspector_PROPERTY_CELL_RENDERER_H_


#include <gtk/gtk.h>


#define GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER            (gtkinspector_property_cell_renderer_get_type())
#define GTKINSPECTOR_PROPERTY_CELL_RENDERER(obj)            (G_TYPE_CHECK_INSTANCE_CAST((obj), GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER, GtkinspectorPropertyCellRenderer))
#define GTKINSPECTOR_PROPERTY_CELL_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST((klass), GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER, GtkinspectorPropertyCellRendererClass))
#define GTKINSPECTOR_IS_PROPERTY_CELL_RENDERER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER))
#define GTKINSPECTOR_IS_PROPERTY_CELL_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER))
#define GTKINSPECTOR_PROPERTY_CELL_RENDERER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS((obj), GTKINSPECTOR_TYPE_PROPERTY_CELL_RENDERER, GtkinspectorPropertyCellRendererClass))

typedef struct _GtkinspectorPropertyCellRendererPrivate GtkinspectorPropertyCellRendererPrivate;

typedef struct
{
  GtkCellRendererText parent;
  GtkinspectorPropertyCellRendererPrivate *priv;
} GtkinspectorPropertyCellRenderer;

typedef struct
{
   GtkCellRendererTextClass parent;

   // Padding for future expansion
   void (*reserved0)(void);
   void (*reserved1)(void);
   void (*reserved2)(void);
   void (*reserved3)(void);

} GtkinspectorPropertyCellRendererClass;


G_BEGIN_DECLS


GType gtkinspector_property_cell_renderer_get_type();
GtkCellRenderer *gtkinspector_property_cell_renderer_new();


G_END_DECLS


#endif // _gtkinspector_PROPERTY_CELL_RENDERER_H_

// vim: set et sw=4 ts=4:
