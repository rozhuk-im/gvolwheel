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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <config.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <glib/gi18n-lib.h>


/* Standard gettext macros. */
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
#  ifndef _
#    define _(String) (String)
#  endif
#  define N_(String) (String)
#endif

#include "gvolwheel.h"


int
main(int argc, char *argv[])
{
	gvw_t gvw;
	GError *error = NULL;
	GOptionContext *context;
	GOptionEntry entries[] = {
		{ "device", 'd', 0, G_OPTION_ARG_STRING, &gvw.device, "Audio device name (e.g. hw:1, /dev/mixer)", "N" },
		{ NULL }
	};

#ifdef ENABLE_NLS
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

//gtk3	gtk_set_locale();
	gtk_init(&argc, &argv);
	memset(&gvw, 0x00, sizeof(gvw));

	/* Parse command line. */
	context = g_option_context_new("- tray icon audio volume mixer");
	g_option_context_add_main_entries(context, entries, GETTEXT_PACKAGE);
	g_option_context_add_group(context, gtk_get_option_group (TRUE));
	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print(_("option parsing failed: %s\n"), error->message);
		exit(1);
	}
	g_option_context_free(context);

	settings_load(&gvw.s);
	if (vol_init(&gvw)) {
		g_printf(_("Error opening mixer device\n"));
		exit(1);
	}
	tray_icon_init(&gvw);

	/* For update icon, if volume changed from other app. */
	g_timeout_add(1000, (GSourceFunc)tray_icon_update, &gvw);

	gtk_main();

	return (0);
}

void
launch_mixer(gvw_p gvw)
{
	char cmd_line[4096];
	snprintf(cmd_line, sizeof(cmd_line), "%s %c",
	    gvw->s.mixer_app_name, '&');
	system(cmd_line);
}
