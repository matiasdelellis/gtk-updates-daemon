gsettings_SCHEMAS = org.gtk.UpdatesDaemon.gschema.xml

@INTLTOOL_XML_NOMERGE_RULE@
@GSETTINGS_RULES@

org.gtk.UpdatesDaemon.gschema.xml.in: org.gtk.UpdatesDaemon.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

CLEANFILES = $(gsettings_SCHEMAS)
