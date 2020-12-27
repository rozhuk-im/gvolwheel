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

#include <sys/param.h>
#include <sys/types.h>
#include <inttypes.h>

#include <config.h>
#include "gvolwheel.h"


static const char *tray_image_stocks[VOLUME_GRAPH_LEVELS] = {
	"audio-volume-muted",
	"audio-volume-low",
	"audio-volume-medium",
	"audio-volume-high"
};


static void
on_tray_icon_menu_config_click(GtkMenuItem *menuitem __unused,
    gpointer user_data)
{
	config_window_show(user_data);
}

static void
on_tray_icon_menu_about_click(GtkMenuItem *menuitem __unused,
    gpointer user_data __unused)
{
	GtkAboutDialog *dlg = GTK_ABOUT_DIALOG(gtk_about_dialog_new());
	gtk_about_dialog_set_program_name(dlg, "GVolWheel");
	gtk_about_dialog_set_version(dlg, VERSION);
	gtk_about_dialog_set_copyright(dlg,
	    "Copyright (c) Dmitry Kosenkov 2011\n"
	    "Copyright (c) 2020 Rozhuk Ivan <rozhuk.im@gmail.com>");
	gtk_about_dialog_set_website(dlg, PACKAGE_URL);
	gtk_about_dialog_set_comments(dlg, "Volume mixer");
	gtk_dialog_run(GTK_DIALOG(dlg));
	gtk_widget_destroy(GTK_WIDGET(dlg));
}

static void
tray_icon_menu_show(gvw_p gvw)
{
	if (!gvw->tray_icon_menu) {
		GtkWidget *mitem;
		gvw->tray_icon_menu = GTK_MENU(gtk_menu_new());

		/* Preferences. */
		mitem = gtk_image_menu_item_new_from_stock("gtk-preferences", NULL);
		g_signal_connect(G_OBJECT(mitem), "activate",
		    G_CALLBACK(on_tray_icon_menu_config_click), gvw);
		gtk_menu_shell_append(GTK_MENU_SHELL(gvw->tray_icon_menu),
		    mitem);
		/* About. */
		mitem = gtk_image_menu_item_new_from_stock("gtk-about", NULL);
		g_signal_connect(G_OBJECT(mitem), "activate",
		    G_CALLBACK(on_tray_icon_menu_about_click), gvw);
		gtk_menu_shell_append(GTK_MENU_SHELL(gvw->tray_icon_menu),
		    mitem);
		/* Separator. */
		gtk_menu_shell_append(GTK_MENU_SHELL(gvw->tray_icon_menu),
		    gtk_separator_menu_item_new());
		/* Quit. */
		mitem  = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
		g_signal_connect(G_OBJECT(mitem), "activate",
		    G_CALLBACK(gtk_main_quit), NULL);
		gtk_menu_shell_append(GTK_MENU_SHELL(gvw->tray_icon_menu),
		    mitem);

		gtk_widget_show_all(GTK_WIDGET(gvw->tray_icon_menu));
	}
	gtk_menu_popup_at_pointer(gvw->tray_icon_menu, NULL);
}


static void
on_tray_icon_scroll(GtkWidget *widget __unused, GdkEventScroll *event,
    gpointer user_data)
{
	switch (event->direction) {
	case GDK_SCROLL_UP:
		vol_up(user_data);
		break;
	case GDK_SCROLL_DOWN:
		vol_down(user_data);
		break;
	}
}

static void
on_tray_icon_click(GtkWidget *widget __unused, GdkEventButton *event,
    gpointer user_data)
{
	switch (event->button) {
	case 2:
		vol_mute_toggle(user_data);
		break;
	case 3:
		tray_icon_menu_show(user_data);
		break;
	}
}

static void
on_tray_icon_press(GtkWidget *widget __unused, GdkEventButton *event,
    gpointer user_data)
{
	if (event->button != 1)
		return;
	switch (event->type) {
	case GDK_BUTTON_PRESS:
		vol_window_toggle(user_data);
		break;
	case GDK_2BUTTON_PRESS:
		launch_mixer(user_data);
		break;
	}
}

void
tray_icon_init(gvw_p gvw)
{
	gvw->tray_icon = gtk_status_icon_new();

	if (gvw->s.gnome_icons) {
		gtk_status_icon_set_from_icon_name(gvw->tray_icon,
		    tray_image_stocks[1]);
	} else {
		tray_icon_reload_pixbufs(gvw);
		gtk_status_icon_set_from_pixbuf(gvw->tray_icon,
		    gvw->tray_pixbufs[2]);
	}

	g_signal_connect(G_OBJECT(gvw->tray_icon), "button-release-event",
	    G_CALLBACK(on_tray_icon_click), gvw);
	g_signal_connect(G_OBJECT(gvw->tray_icon), "button-press-event",
	    G_CALLBACK(on_tray_icon_press), gvw);
	g_signal_connect(G_OBJECT(gvw->tray_icon), "scroll-event",
	    G_CALLBACK(on_tray_icon_scroll), gvw);

	tray_icon_update(gvw);

	gtk_status_icon_set_visible(gvw->tray_icon, TRUE);
}


void
tray_icon_reload_pixbufs(gvw_p gvw)
{
	gchar *pixmap_path, img_filename[1024];

	pixmap_path = g_build_filename(PACKAGE_DATA_DIR, "pixmaps",
	    PACKAGE_NAME, NULL);
	for (size_t i = 0; i < VOLUME_GRAPH_LEVELS; i ++) {
		/* Try to load from profile dir. */
		g_snprintf(img_filename, sizeof(img_filename), "%s/%s/%s/%s/%s.png",
		    g_get_home_dir(), ".config", PACKAGE_NAME, "pixmaps",
		    tray_image_stocks[i]);
		/* If not exist - load from package defauilt. */
		if (!g_file_test(img_filename, G_FILE_TEST_EXISTS)) {
			snprintf(img_filename, sizeof(img_filename),
			    "%s/%s.png",
			    pixmap_path, tray_image_stocks[i]);
		}
		/* XXX: need some free res here? */
		gvw->tray_imgs[i] = GTK_IMAGE(gtk_image_new_from_file(img_filename));
		gvw->tray_pixbufs[i] = gtk_image_get_pixbuf(gvw->tray_imgs[i]);
	}
	g_free(pixmap_path);
}


gboolean
tray_icon_update(gvw_p gvw)
{
	size_t i;
	uint32_t cur_vol = vol_get(gvw);
	const uint32_t vol_lvls[VOLUME_GRAPH_LEVELS] = { 0, 33, 66, 100 };
	GdkPixbuf *cur_icon_pixbuf = NULL;
	const char *cur_icon_name = "";
	char volchar[16];

	/* Get current icon. */
	switch (gtk_status_icon_get_storage_type(gvw->tray_icon)) {
	case GTK_IMAGE_PIXBUF:
		cur_icon_pixbuf = gtk_status_icon_get_pixbuf(gvw->tray_icon);
		break;
	case GTK_IMAGE_STOCK:
		cur_icon_name = gtk_status_icon_get_icon_name(gvw->tray_icon);
		break;
	}

	/* Use gnome if enabled or if no pixbufs loaded. */
	if (gvw->s.gnome_icons || gvw->tray_pixbufs[0] == NULL) {
		for (i = 0; i < VOLUME_GRAPH_LEVELS; i ++) {
			if (vol_lvls[i] < cur_vol)
				continue;
			if (strcmp(cur_icon_name, tray_image_stocks[i]) == 0)
				break; /* No need to update. */
			gtk_status_icon_set_from_icon_name(gvw->tray_icon,
			    tray_image_stocks[i]);
			break;
		}
	} else {
		for (i = 0; i < VOLUME_GRAPH_LEVELS; i ++) {
			if (vol_lvls[i] < cur_vol)
				continue;
			if (cur_icon_pixbuf == gvw->tray_pixbufs[i])
				break; /* No need to update. */
			gtk_status_icon_set_from_pixbuf(gvw->tray_icon,
			    gvw->tray_pixbufs[i]);
			break;
		}
	}

	if (!gvw->s.show_tooltip) {
		gtk_status_icon_set_tooltip_text(gvw->tray_icon, "");
	} else {
		snprintf(volchar, sizeof(volchar), "%i%%", cur_vol);
		gtk_status_icon_set_tooltip_text(gvw->tray_icon, volchar);
	}

	return (TRUE);
}
