all:
	gcc -o gtk-updates-daemon \
	       gtk-updates-daemon.c \
	       gud-sync-helper.c \
	       gud-sync-helper.h \
	       gud-pk-progress-bar.c \
	       gud-pk-progress-bar.h \
	       `pkg-config --libs --cflags gtk+-3.0 packagekit-glib2 libnotify`
