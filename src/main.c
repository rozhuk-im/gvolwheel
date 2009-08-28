/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) Dmitry Kosenkov 2008 <junker@front.ru>
 * 
 * main.c is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * main.c is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
 
#include <config.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif


#include "eggtrayicon.h"
#include "actions.h"
#include "callbacks.h"
 
extern gboolean opt_gnome_icons; 
int mixer_fd;
EggTrayIcon *egg_tray_icon; 
GdkPixbuf *tray_pixbufs[4]; 
GtkImage *tray_image;
GtkEventBox *tray_box;
const char *tray_image_stocks[] = {
	 "audio-volume-muted",
	 "audio-volume-low",
	 "audio-volume-medium",
	 "audio-volume-high"
 	 };


 

static EggTrayIcon *create_egg_tray_icon() 
{
	tray_box = GTK_EVENT_BOX(gtk_event_box_new());
	if (opt_gnome_icons==TRUE)
		tray_image = GTK_IMAGE(gtk_image_new_from_icon_name (tray_image_stocks[1],GTK_ICON_SIZE_BUTTON));
	else LoadPixbufs(), tray_image = gtk_image_new_from_pixbuf (tray_pixbufs[2]);
	
	egg_tray_icon = egg_tray_icon_new("GVolwheel");
	gtk_container_add(GTK_CONTAINER(tray_box),GTK_WIDGET(tray_image));
	gtk_container_add(GTK_CONTAINER(egg_tray_icon), GTK_WIDGET(tray_box));	

	update_tray_image();

	g_signal_connect(G_OBJECT(tray_box),"button-release-event",G_CALLBACK(on_tray_icon_click), NULL);
	g_signal_connect(G_OBJECT(tray_box),"button-press-event",G_CALLBACK(on_tray_icon_press), NULL);
	g_signal_connect(G_OBJECT(tray_box),"scroll-event",G_CALLBACK(on_tray_icon_scroll), NULL);
	
	gtk_widget_show_all (GTK_WIDGET(egg_tray_icon));


	return egg_tray_icon;
}

int main (int argc, char *argv[])
{

#ifdef ENABLE_NLS
//	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
//	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
//	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_set_locale ();
	gtk_init (&argc, &argv);
	
	mixer_fd = open ("/dev/mixer", R_OK+W_OK, 0);
  	if (mixer_fd < 0)
    	g_printf ("Error opening mixer device\n"), exit (1);
	
	load_config ();
	egg_tray_icon = create_egg_tray_icon();
	
	gtk_main ();

	close(mixer_fd);
	return 0;
}
