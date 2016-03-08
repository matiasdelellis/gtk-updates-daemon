gsettings_SCHEMAS = org.huayra.UpdatesNotifier.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

org.huayra.UpdatesNotifier.gschema.xml.in: org.huayra.UpdatesNotifier.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = pk-updates-notifier-autostart.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

CLEANFILES = $(gsettings_SCHEMAS)
