bin_PROGRAMS = gtk-updates-daemon
gtk_updates_daemon_SOURCES = 	\
	gtk-updates-daemon.c \
	gud-sync-helper.c \
	gud-sync-helper.h \
	gud-pk-progress-bar.c \
	gud-pk-progress-bar.h

gtk_updates_daemon_CFLAGS = \
	$(GTK_CFLAGS) \
	$(PACKAGEKIT_CFLAGS) \
	$(NOTIFY_CFLAGS)

gtk_updates_daemon_LDADD = \
	$(GTK_LIBS) \
	$(PACKAGEKIT_LIBS) \
	$(NOTIFY_LIBS)

CLEANFILES = *~

