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
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>

#include "gvolwheel.h"


void
settings_load(gvw_settings_p gvws)
{
	gchar *config_filename;
	GKeyFile *config = g_key_file_new();

	config_filename = g_build_filename(g_get_home_dir(),
	    ".config", PACKAGE_NAME, "gvolwheel.conf", NULL);
	if (g_key_file_load_from_file(config, config_filename,
	    G_KEY_FILE_NONE, NULL) == FALSE) {
		g_free(config_filename);
		/* Defauilt settings. */
		gvws->channel = MIXER_CHANNEL_MAIN;
		gvws->incr = 3;
		gvws->gnome_icons = FALSE;
		gvws->show_tooltip = FALSE;
		strcpy(gvws->mixer_app_name, "gnome-alsamixer");
		return;
	}
	g_free(config_filename);

	gvws->channel = (uint32_t)g_key_file_get_integer(config,
	    "Options", "Channel", NULL);
	if (gvws->channel > MIXER_CHANNEL_PCM) {
		gvws->channel = MIXER_CHANNEL_MAIN;
	}
	strcpy(gvws->mixer_app_name, g_key_file_get_string(config,
	    "Options", "Mixer", NULL));
	gvws->incr = (uint32_t)g_key_file_get_integer(config,
	    "Options", "Incr", NULL);
	gvws->gnome_icons = g_key_file_get_boolean(config,
	    "Options", "Gnome_Icons", NULL);
	gvws->show_tooltip = g_key_file_get_boolean(config,
	    "Options", "Show_Tooltip", NULL);

	g_key_file_free(config);
}

void
settings_save(gvw_settings_p gvws)
{
	gchar *config_filename, *config_dir, *buffer;
	GKeyFile *config = g_key_file_new();

	config_filename = g_build_filename(g_get_home_dir(),
	    ".config", PACKAGE_NAME, "gvolwheel.conf", NULL);

	/* XXX: need free? */
	config_dir = g_path_get_dirname(config_filename);
	g_mkdir(config_dir, 0755);

	g_key_file_set_integer(config, "Options", "Channel",
	    (gint)gvws->channel);
	g_key_file_set_string(config, "Options", "Mixer",
	    gvws->mixer_app_name);
	g_key_file_set_integer(config, "Options", "Incr",
	    (gint)gvws->incr);
	g_key_file_set_boolean(config, "Options", "Gnome_Icons",
	    gvws->gnome_icons);
	g_key_file_set_boolean(config, "Options", "Show_Tooltip",
	    gvws->show_tooltip);
	
	buffer = g_key_file_to_data(config, NULL, NULL);
	g_key_file_free(config);
	
	g_file_set_contents(config_filename, buffer,
	    (gssize)strlen(buffer), NULL);

	g_free(buffer);
	g_free(config_filename);
}
