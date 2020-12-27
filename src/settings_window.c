/*-
 * Copyright (C) Dmitry Kosenkov 2011 <junker@front.ru>
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

#include <config.h>
#include <glib/gi18n-lib.h>

#include "gvolwheel.h"

static GtkWindow *confwin = NULL;
static GtkComboBox *confwin_combo_channel = NULL;
static GtkEntry *confwin_entry_mixer = NULL;
static GtkSpinButton *confwin_spin_incr = NULL;
static GtkCheckButton *confwin_gnome_icons = NULL, *confwin_show_tooltip = NULL;


static void
on_config_window_close(GtkButton *button __unused, gpointer user_data __unused)
{
	gtk_widget_destroy(GTK_WIDGET(confwin));
	confwin = NULL;
	confwin_combo_channel = NULL;
	confwin_entry_mixer = NULL;
	confwin_spin_incr = NULL;
	confwin_gnome_icons = NULL;
	confwin_show_tooltip = NULL;
}

static void
on_config_window_save(GtkButton *button, gpointer user_data)
{
	gvw_p gvw = user_data;

	gvw->s.channel = (uint32_t)gtk_combo_box_get_active(confwin_combo_channel);
	gvw->s.incr = (uint32_t)gtk_spin_button_get_value_as_int(confwin_spin_incr);
	gvw->s.gnome_icons = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(confwin_gnome_icons));
	gvw->s.show_tooltip = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(confwin_show_tooltip));
	strcpy(gvw->s.mixer_app_name, gtk_entry_get_text(confwin_entry_mixer));
	on_config_window_close(button, user_data);

	settings_save(&gvw->s);
	tray_icon_reload_pixbufs(gvw);
	tray_icon_update(gvw);
}

void config_window_show(gvw_p gvw)
{
	if (confwin)
		return;
	confwin = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
	gtk_window_set_position(confwin, GTK_WIN_POS_CENTER);
	gtk_window_set_destroy_with_parent(confwin, FALSE);

	gtk_container_set_border_width(GTK_CONTAINER(confwin), 10);

	GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	GtkWidget *hbox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

	confwin_combo_channel = GTK_COMBO_BOX(gtk_combo_box_text_new());
	for (size_t i = 0; i < nitems(mixer_channel_name); i ++) {
		gtk_combo_box_text_append(
		    GTK_COMBO_BOX_TEXT(confwin_combo_channel), NULL,
		    mixer_channel_name[i]);
	}
	gtk_combo_box_set_active(confwin_combo_channel,
	    (gint)gvw->s.channel);
	gtk_box_pack_start(GTK_BOX(hbox1),
	    GTK_WIDGET(gtk_label_new("Default channel")), 0, 0, 5);
	gtk_box_pack_end(GTK_BOX(hbox1),
	    GTK_WIDGET(confwin_combo_channel), 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox1), 0, 0, 0);

	GtkWidget *hbox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);

	confwin_entry_mixer = GTK_ENTRY(gtk_entry_new());
	gtk_entry_set_text(confwin_entry_mixer, gvw->s.mixer_app_name);
	gtk_box_pack_start(GTK_BOX(hbox2),
	    GTK_WIDGET(gtk_label_new(_("Default mixer"))), 0, 0, 5);
	gtk_box_pack_end(GTK_BOX(hbox2),
	    GTK_WIDGET(confwin_entry_mixer), 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox2), 0, 0, 0);

	GtkWidget *hbox3 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	confwin_spin_incr = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1, 99, 1));
	gtk_spin_button_set_value(confwin_spin_incr, gvw->s.incr);
	gtk_box_pack_start(GTK_BOX(hbox3),
	    GTK_WIDGET(gtk_label_new(_("Volume incrementation"))), 0, 0, 5);
	gtk_box_pack_end(GTK_BOX(hbox3),
	    GTK_WIDGET(confwin_spin_incr), 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox3), 0, 0, 0);

	GtkWidget *hbox4 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	confwin_gnome_icons = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Use Gnome Icons")));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(confwin_gnome_icons),
	    gvw->s.gnome_icons);
	gtk_box_pack_end(GTK_BOX(hbox4),
	    GTK_WIDGET(confwin_gnome_icons), 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox4), 0, 0, 0);

	GtkWidget *hbox5 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	confwin_show_tooltip = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(_("Show tooltip")));
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(confwin_show_tooltip),
	    gvw->s.show_tooltip);
	gtk_box_pack_end(GTK_BOX(hbox5),
	    GTK_WIDGET(confwin_show_tooltip), 0, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox5), 0, 0, 0);

	GtkWidget *hbox6 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
	GtkButton *btn_cancel = GTK_BUTTON(gtk_button_new_from_stock("gtk-cancel"));
	g_signal_connect(G_OBJECT(btn_cancel), "clicked",
	    G_CALLBACK(on_config_window_close), gvw);
	GtkButton *btn_ok = GTK_BUTTON(gtk_button_new_from_stock("gtk-save"));
	g_signal_connect(G_OBJECT(btn_ok), "clicked",
	    G_CALLBACK(on_config_window_save), gvw);

	gtk_box_pack_start(GTK_BOX(hbox6), GTK_WIDGET(btn_cancel), 0, 0, 5);
	gtk_box_pack_end(GTK_BOX(hbox6), GTK_WIDGET(btn_ok),0, 0, 5);
	gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(hbox6),0, 0, 0);

	gtk_container_add(GTK_CONTAINER(confwin), GTK_WIDGET(vbox));
	gtk_widget_show_all(GTK_WIDGET(confwin));
}
