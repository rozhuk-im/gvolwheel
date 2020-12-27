/*-
 * Copyright (C) Dmitry Kosenkov 2008 <junker@front.ru>
 * Copyright (c) 2020 Rozhuk Ivan <rozhuk.im@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <config.h>
#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>

#include "gvolwheel.h"


static void
on_vol_value_change(GtkRange *range, gpointer user_data)
{
	vol_set(user_data, (uint32_t)gtk_range_get_value(range));
}

static void
on_vol_mixer_clicked(GtkButton *button __unused, gpointer user_data) {
	
	launch_mixer(user_data);
}

static gboolean
vol_window_close(GtkWidget *widget, GdkEvent *event __unused,
    gpointer user_data)
{
	gvw_p gvw = user_data;

	gtk_widget_destroy(widget);
	gvw->vol_window = NULL;

	return (TRUE);
}

void
vol_window_toggle(gvw_p gvw)
{
	if (GTK_IS_WINDOW(gvw->vol_window)) {
		vol_window_close(GTK_WIDGET(gvw->vol_window), NULL, gvw);
		return;
	}

	gvw->vol_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_container_set_border_width(GTK_CONTAINER(gvw->vol_window), 5);
	gtk_window_set_decorated(gvw->vol_window, FALSE);
	gtk_window_set_position(gvw->vol_window, GTK_WIN_POS_MOUSE);
	gtk_window_set_skip_taskbar_hint(gvw->vol_window, TRUE);
	gtk_window_set_skip_pager_hint(gvw->vol_window, TRUE);
	g_signal_connect(G_OBJECT(gvw->vol_window), "focus-out-event",
	    G_CALLBACK(vol_window_close), gvw);

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	GtkButton *btn = GTK_BUTTON(gtk_button_new_with_label(_("Mixer")));
	g_signal_connect(G_OBJECT(btn), "clicked",
	    G_CALLBACK(on_vol_mixer_clicked), gvw);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(btn), 0, 0, 0);

	gtk_box_pack_start(GTK_BOX(vbox),
	    gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), 0, 0, 0);

	GtkWidget *vscale = gtk_scale_new_with_range(GTK_ORIENTATION_VERTICAL,
	    0, 100, 1);
	gtk_scale_set_digits(GTK_SCALE(vscale), 0);
	gtk_scale_set_value_pos(GTK_SCALE(vscale), GTK_POS_BOTTOM);
	gtk_range_set_inverted(GTK_RANGE(vscale), TRUE);
	gtk_widget_set_size_request(GTK_WIDGET(vscale), 0, 120);

	gtk_range_set_value(GTK_RANGE(vscale), vol_get(gvw));
	g_signal_connect(vscale, "value-changed",
	    G_CALLBACK(on_vol_value_change), gvw);

	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(vscale), 0, 0, 0);

	GtkLabel *label = GTK_LABEL(gtk_label_new("Mixer_channel"));
	gtk_label_set_label(label, mixer_channel_name[gvw->s.channel]);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(label), 0, 0, 0);

	gtk_container_add(GTK_CONTAINER(gvw->vol_window), GTK_WIDGET(vbox));

	gtk_widget_show_all(GTK_WIDGET(vbox));
	gtk_window_set_modal(gvw->vol_window, TRUE);
	gtk_window_present(gvw->vol_window);
	gtk_window_set_focus(gvw->vol_window, NULL);
}

