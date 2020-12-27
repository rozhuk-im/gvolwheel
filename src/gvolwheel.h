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

#include <sys/param.h>
#include <sys/types.h>
#include <inttypes.h>
#include <config.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <glib/gi18n-lib.h>

#ifndef nitems
#define	nitems(x)	(sizeof((x)) / sizeof((x)[0]))
#endif


#define VOLUME_GRAPH_LEVELS	4 /* Mute, low, mid, high. */

#define MIXER_CHANNEL_MAIN	0
#define MIXER_CHANNEL_PCM	1
static const char *mixer_channel_name[2] = {
	"Main",
	"PCM"
};

typedef struct gvolwheel_settings_s { /* Settings. */
	uint32_t	channel; /* MIXER_CHANNEL_* */
	uint32_t	incr;
	int		gnome_icons;
	int		show_tooltip;
	char		mixer_app_name[1024];
} gvw_settings_t, *gvw_settings_p;


typedef struct gvolwheel_s { /* All state. */
	char		*device;
	void		*sound_backend_ctx;
	uint32_t	sound_mute_val; /* Keep vol on mute. */
	GtkStatusIcon	*tray_icon;
	GtkImage	*tray_imgs[VOLUME_GRAPH_LEVELS];
	GdkPixbuf	*tray_pixbufs[VOLUME_GRAPH_LEVELS];
	GtkMenu 	*tray_icon_menu;
	GtkWindow	*vol_window;
	gvw_settings_t	s;
} gvw_t, *gvw_p;


void launch_mixer(gvw_p gvw);

void settings_load(gvw_settings_p gvws);
void settings_save(gvw_settings_p gvws);

void tray_icon_init(gvw_p gvw);
gboolean tray_icon_update(gvw_p gvw);
void tray_icon_reload_pixbufs(gvw_p gvw);

void config_window_show(gvw_p gvw);
void vol_window_toggle(gvw_p gvw);

int vol_init(gvw_p gvw);
uint32_t vol_get(gvw_p gvw);
void vol_up(gvw_p gvw);
void vol_down(gvw_p gvw);
void vol_mute_toggle(gvw_p gvw);
void vol_set(gvw_p gvw, uint32_t value);
