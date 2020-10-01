@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

gsettingsschema_in_files = org.huayra.UpdatesNotifier.gschema.xml.in
gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)

autostartdir = $(sysconfdir)/xdg/autostart
autostart_in_files = pk-updates-notifier-autostart.desktop.in
autostart_DATA = $(autostart_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST= \
    $(gsettingsschema_in_files) \
    $(autostart_in_files)

CLEANFILES = \
    $(gsettings_SCHEMAS) \
    $(autostart_DATA)