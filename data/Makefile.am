gsettings_SCHEMAS = org.gtk.UpdatesDaemon.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

org.gtk.UpdatesDaemon.gschema.xml.in: org.gtk.UpdatesDaemon.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = gtk-updates-daemon-autostart.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

CLEANFILES = $(gsettings_SCHEMAS)
