## Process this file with automake to produce Makefile.in

## Created by Anjuta

gladedir = $(datadir)/gvolwheel/glade
AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(GTK_CFLAGS) \
        ${ALSA_CFLAGS}

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = gvolwheel

if BACKEND_ALSA
BACKEND = volume_alsa.c
else
BACKEND = volume_oss.c
endif

gvolwheel_SOURCES = \
	main.c \
	settings.c \
	gvolwheel.h \
	volume.c \
	trayicon.c \
	settings_window.c \
	volume_window.c \
        $(BACKEND)

gvolwheel_LDFLAGS = \
	-Wl,--export-dynamic

gvolwheel_LDADD = $(GTK_LIBS) ${ALSA_LIBS}

EXTRA_DIST = $(glade_DATA)
