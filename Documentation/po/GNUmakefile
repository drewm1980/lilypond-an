depth = ../..

POT_FILES = $(call src-wildcard,*.pot)
PO_FILES = $(call src-wildcard,*.po)
MO_FILES = $(addprefix $(outdir)/, $(PO_FILES:.po=.mo))
HELP_CATALOGS = $(PO_FILES:%.po=%)
CATALOGS = $(HELP_CATALOGS:$(DOMAIN)=)
STEPMAKE_TEMPLATES=podir

include $(depth)/make/stepmake.make

doc-localedir=$(outdir)
DOMAIN=lilypond-doc

messages: $(MO_FILES)
	for i in $(CATALOGS); do \
	  mkdir -p $(doc-localedir)/$$i/LC_MESSAGES; \
	  cp $(outdir)/$$i.mo $(doc-localedir)/$$i/LC_MESSAGES/$(DOMAIN).mo; \
	done

po-update:
	$(PYTHON) $(depth)/buildscripts/texi-langutils.py -d $(outdir) -o doc.pot --gettext ../$(depth)/Documentation/user/lilypond.tely
	msgmerge -U lilypond-doc.pot $(outdir)/doc.pot
	for i in $(CATALOGS); do \
	  msgmerge -U $$i.po lilypond-doc.pot; \
	done