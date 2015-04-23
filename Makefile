SUBDIRS = src

subdirs:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir; \
	done

install:
	cp src/gtk-updates-daemon-autostart.desktop /etc/xdg/autostart/
	cp src/gtk-updates-daemon /usr/bin

uninstall:
	rm /etc/xdg/autostart/gtk-updates-daemon-autostart.desktop
	rm /usr/bin/gtk-updates-daemon

