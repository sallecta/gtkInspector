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
#include "widget-tree.h"

static void
on_inspect_widget(GtkWidget *grab_window,
                  GdkEventButton *event,
                  GtkinspectorWindow *gtkinspector)
{
  gdk_device_ungrab (gtk_get_current_event_device (), event->time);
  gtk_widget_hide(gtkinspector->highlight_window);

  if (gtkinspector->selected_window != NULL)
    {
      GtkWidget *toplevel = NULL;
      GtkWidget *widget = NULL;

      gdk_window_get_user_data(
            gdk_window_get_toplevel(gtkinspector->selected_window),
            (gpointer*)&toplevel);

      gdk_window_get_user_data (gtkinspector->selected_window, (gpointer*)&widget);

      if (toplevel)
        {
          gtkinspector_widget_tree_scan (GTKINSPECTOR_WIDGET_TREE (gtkinspector->widget_tree),
                                     toplevel);
        }

      if (widget)
        {
          gtkinspector_widget_tree_select_object (GTKINSPECTOR_WIDGET_TREE (gtkinspector->widget_tree),
                                              G_OBJECT (widget));
        }
    }
}

static void
on_highlight_window_show (GtkWidget *window, GtkinspectorWindow *gtkinspector)
{
  if (gtk_widget_is_composited (gtkinspector->window))
    {
      gtk_widget_set_opacity (gtkinspector->highlight_window, 0.2);
    }
  else
    {
      /*
       * TODO: Do something different when there's no compositing manager.
       *         Draw a border or something.
       */
    }
}

static void
ensure_highlight_window (GtkinspectorWindow *gtkinspector)
{
  GdkRGBA color;

  if (gtkinspector->highlight_window != NULL)
    return;

  color.red = 0;
  color.green = 0;
  color.blue = 65535;
  color.alpha = 1;

  gtkinspector->highlight_window = gtk_window_new (GTK_WINDOW_POPUP);
  gtk_widget_override_background_color (gtkinspector->highlight_window,
                                        GTK_STATE_NORMAL,
                                        &color);

  g_signal_connect (gtkinspector->highlight_window, "show", G_CALLBACK (on_highlight_window_show), gtkinspector);
}

static void
on_highlight_widget(GtkWidget *grab_window,
                    GdkEventMotion *event,
                    GtkinspectorWindow *gtkinspector)
{
  GdkWindow *selected_window;
  gint x, y, width, height;

  ensure_highlight_window (gtkinspector);

  gtk_widget_hide (gtkinspector->highlight_window);

  selected_window = gdk_device_get_window_at_position (gtk_get_current_event_device (),
                                                       NULL,
                                                       NULL);
  if (selected_window == NULL)
    {
      /* This window isn't in-process. Ignore it. */
      gtkinspector->selected_window = NULL;
      return;
    }

  if (gdk_window_get_toplevel(selected_window) == gtk_widget_get_window(gtkinspector->window))
    {
       /* Don't hilight things in the gtkinspector window */
        gtkinspector->selected_window = NULL;
        return;
    }

  gtkinspector->selected_window = selected_window;

  gdk_window_get_origin (selected_window, &x, &y);
  width = gdk_window_get_width (selected_window);
  height = gdk_window_get_height (selected_window);

  gtk_window_move (GTK_WINDOW (gtkinspector->highlight_window), x, y);
  gtk_window_resize (GTK_WINDOW (gtkinspector->highlight_window), width, height);
  gtk_widget_show (gtkinspector->highlight_window);
}

static void
on_inspect_button_release (GtkWidget *button,
                           GdkEventButton *event,
                           GtkinspectorWindow *gtkinspector)
{
  GdkCursor *cursor;
  GdkEventMask events;

  events = GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK;

  if (gtkinspector->grab_window == NULL)
    {
      gtkinspector->grab_window = gtk_window_new (GTK_WINDOW_POPUP);
      gtk_widget_show(gtkinspector->grab_window);
      gtk_window_resize (GTK_WINDOW (gtkinspector->grab_window), 1, 1);
      gtk_window_move (GTK_WINDOW (gtkinspector->grab_window), -100, -100);
      gtk_widget_add_events (gtkinspector->grab_window, events);

      g_signal_connect (gtkinspector->grab_window, "button_release_event", G_CALLBACK (on_inspect_widget), gtkinspector);
      g_signal_connect (gtkinspector->grab_window, "motion_notify_event", G_CALLBACK(on_highlight_widget), gtkinspector);
    }

  cursor = gdk_cursor_new_for_display (gtk_widget_get_display (button), GDK_CROSSHAIR);
  gdk_device_grab (gtk_get_current_event_device (),
                   gtk_widget_get_window (gtkinspector->grab_window),
                   GDK_OWNERSHIP_WINDOW,
                   FALSE,
                   events,
                   cursor,
                   event->time);
  g_object_unref (cursor);
}


GtkWidget *
gtkinspector_inspect_button_new(GtkinspectorWindow *gtkinspector)
{
    GtkWidget *button;

    button = gtk_button_new_from_icon_name ("find", GTK_ICON_SIZE_BUTTON);
    gtk_widget_set_tooltip_text (button, "Inspect");
    g_signal_connect(G_OBJECT(button), "button_release_event",
                     G_CALLBACK(on_inspect_button_release), gtkinspector);

    return button;
}

static gboolean
on_flash_timeout(GtkinspectorWindow *gtkinspector)
{
    gtkinspector->flash_count++;

    if (gtkinspector->flash_count == 8)
    {
        gtkinspector->flash_cnx = 0;
        return FALSE;
    }

    if (gtkinspector->flash_count % 2 == 0)
    {
        if (gtk_widget_get_visible(gtkinspector->highlight_window))
            gtk_widget_hide(gtkinspector->highlight_window);
        else
            gtk_widget_show(gtkinspector->highlight_window);
    }

    return TRUE;
}

void
gtkinspector_flash_widget(GtkinspectorWindow *gtkinspector, GtkWidget *widget)
{
    gint x, y, width, height;
    GdkWindow *parent_window;

    if (!gtk_widget_get_visible(widget) || !gtk_widget_get_mapped(widget))
        return;

    ensure_highlight_window(gtkinspector);

    parent_window = gtk_widget_get_parent_window(widget);
    if (parent_window != NULL) {
    	GtkAllocation alloc;
    	gtk_widget_get_allocation(widget, &alloc);
    	
        gdk_window_get_origin(parent_window, &x, &y);
        x += alloc.x;
        y += alloc.y;

        width = alloc.width;
        height = alloc.height;

        gtk_window_move(GTK_WINDOW(gtkinspector->highlight_window), x, y);
        gtk_window_resize(GTK_WINDOW(gtkinspector->highlight_window), width, height);
        gtk_widget_show(gtkinspector->highlight_window);

        if (gtkinspector->flash_cnx != 0)
            g_source_remove(gtkinspector->flash_cnx);

        gtkinspector->flash_count = 0;
        gtkinspector->flash_cnx = g_timeout_add(150, (GSourceFunc)on_flash_timeout,
                                            gtkinspector);
    }
}

// vim: set et sw=4 ts=4:
