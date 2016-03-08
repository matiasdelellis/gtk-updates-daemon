bin_PROGRAMS = pk-updates-notifier

AM_CPPFLAGS = \
	-I$(top_srcdir)                       \
	-I$(top_srcdir)/src                   \
	-DG_LOG_DOMAIN=\"pk-updates-notifier\" \
	-DGUD_LOCALE_DIR=\""$(datadir)/locale"\"

pk_updates_notifier_SOURCES = 	\
	pk-updates-notifier.c \
	gud-sync-helper.c \
	gud-sync-helper.h \
	gud-pk-progress-bar.c \
	gud-pk-progress-bar.h

pk_updates_notifier_CFLAGS = \
	$(GTK_CFLAGS) \
	$(PACKAGEKIT_CFLAGS) \
	$(NOTIFY_CFLAGS)

pk_updates_notifier_LDADD = \
	$(GTK_LIBS) \
	$(PACKAGEKIT_LIBS) \
	$(NOTIFY_LIBS)

CLEANFILES = *~

