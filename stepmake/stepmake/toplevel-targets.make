.PHONY: install-doc uninstall-doc
install-doc:
uninstall-doc:

local-dist: configure

local-distclean:
	rm -f config.hh config.make Makefile GNUmakefile \
		config.cache config.status config.log index.html \
		stepmake/stepmake/stepmake stepmake/stepmake/bin
	rm -rf autom4te.cache
	rm -rf $(outdir)

local-maintainerclean:
	rm -f configure

GNUmakefile: GNUmakefile.in
	$(MAKE) INFILE=$< OUTFILE=$@ -f $(stepdir)/automatically-generated.sub.make

ifneq ($(PACKAGE),STEPMAKE)
aclocal.m4: $(stepmake)/aclocal.m4
	$(MAKE) INFILE=$< OUTFILE=$@ LINECOMMENT=dnl -f $(stepdir)/automatically-generated.sub.make

autogen.sh: $(stepmake)/autogen.sh
	$(MAKE) INFILE=$< OUTFILE=$@ LINECOMMENT=\# -f $(stepdir)/automatically-generated.sub.make
	chmod +x autogen.sh
endif


$(package-icon):
	$(MAKE) -C Documentation/logo icon


top-doc:
	$(MAKE) -C Documentation/topdocs/ README_TOP_FILES="$(TOPDOC_FILES)" txt-files


$(README_TXT_FILES): top-doc

local-clean:

install:
	$(LOOP)
ifeq ($(strip $(SRCMAKE)),)
	$(MAKE) final-install
endif

local-dist: top-doc

dist:
	rm -rf $(distdir)
	$(MAKE) local-dist $(distdir)
	chmod -R a+r $(distdir)
	chmod  a+x `find $(distdir) -type d -print`
	(cd ./$(depth)/$(outdir); $(TAR) -cf -  --owner=0 --group=0 $(DIST_NAME) | gzip -9 > $(DIST_NAME).tar.gz)
	rm -rf $(distdir)/

local-help:
	@echo "  config          rerun configure"
	@echo "  dist            roll tarball: $(depth)/$(outdir)/$(distname).tar.gz"
	@echo "  distclean       also remove configure output"
	@echo "  cvs-clean       also remove out directories and generated files"
	@echo "  maintainerclean also remove distributed generated files"
	@echo "  po              make new translation Portable Object database"
	@echo "  po-replace      do po-update and replace catalogs with msgmerged versions"
	@echo "  po-update       update translation Portable Object database"
	@echo
	@echo "  install-doc     install website documentation in"
	@echo "                    (webdir=$(webdir))"
	@echo "                    and Info documentation with images"
	@echo "  uninstall-doc   remove installed documentation with images"
	@echo "  info            build Info documentation with images"
	@echo "  install-info    install Info documentation with images"
	@echo
	@echo "Some more targets are available for testing changes:"
	@echo "  test-baseline"
	@echo "  check"
	@echo "  test-redo"
	@echo "  test-clean"
	@echo
	@echo "  For more information on these targets, see"
	@echo "    \`Testing LilyPond' in the Contributor's Guide."
	@echo

