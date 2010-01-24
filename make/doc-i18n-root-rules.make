$(outdir)/%.texi: $(src-dir)/%.texi
	cp -p $< $@

$(top-build-dir)/Documentation/$(outdir)/%/index.$(ISOLANG).html: $(outdir)/%.texi $(XREF_MAPS_DIR)/%.$(ISOLANG).xref-map $(TRANSLATION_LILY_IMAGES)
	mkdir -p $(dir $@)
	mkdir -p $(outdir)/$*
	DEPTH=$(depth)/../ $(TEXI2HTML) $(TEXI2HTML_SPLIT) $(TEXI2HTML_FLAGS) --output=$(outdir)/$* $<
	find $(outdir)/$* -name '*.html' | xargs grep -L 'UNTRANSLATED NODE: IGNORE ME' | sed 's!$(outdir)/!!g' | xargs $(buildscript-dir)/mass-link --prepend-suffix .$(ISOLANG) hard $(outdir) $(top-build-dir)/Documentation/$(outdir)

$(top-build-dir)/Documentation/$(outdir)/%-big-page.$(ISOLANG).html: $(outdir)/%.texi $(XREF_MAPS_DIR)/%.$(ISOLANG).xref-map $(TRANSLATION_LILY_IMAGES)
	DEPTH=$(depth) $(TEXI2HTML) -D bigpage $(TEXI2HTML_FLAGS) --output=$@ $<

$(top-build-dir)/Documentation/$(outdir)/%.$(ISOLANG).html: $(outdir)/%.texi $(XREF_MAPS_DIR)/%.$(ISOLANG).xref-map $(outdir)/version.itexi $(outdir)/weblinks.itexi
	DEPTH=$(depth) $(TEXI2HTML) $(TEXI2HTML_FLAGS) --output=$@ $<

$(top-build-dir)/Documentation/$(outdir)/%.$(ISOLANG).pdf: $(outdir)/%.texi
	cd $(outdir) && \
	    texi2pdf $(TEXI2PDF_FLAGS) $(TEXINFO_PAPERSIZE_OPTION) $*.texi && \
	    mkdir -p $(dir $@) && mv $*.pdf $@

$(outdir)/version.%: $(top-src-dir)/VERSION
	$(PYTHON) $(top-src-dir)/scripts/build/create-version-itexi.py > $@

$(outdir)/weblinks.%: $(top-src-dir)/VERSION
	$(PYTHON) $(top-src-dir)/scripts/build/create-weblinks-itexi.py > $@

$(outdir)/%.png: $(top-build-dir)/Documentation/$(outdir)/%.png
	ln -f $< $@

$(XREF_MAPS_DIR)/%.$(ISOLANG).xref-map: $(outdir)/%.texi $(XREF_MAPS_DIR)/%.xref-map
	$(buildscript-dir)/extract_texi_filenames -o $(XREF_MAPS_DIR) $(XREF_MAP_FLAGS) --master-map-file=$(XREF_MAPS_DIR)/$*.xref-map $<

$(MASTER_TEXI_FILES): $(ITELY_FILES) $(ITEXI_FILES) $(outdir)/pictures

$(outdir)/pictures:
	$(MAKE) -C $(top-build-dir)/Documentation/pictures WWW-1
	ln -sf $(top-build-dir)/Documentation/pictures/$(outdir) $@

$(TRANSLATION_LILY_IMAGES): $(MASTER_TEXI_FILES)
	find $(outdir) \( -name 'lily-*.png' -o -name 'lily-*.ly' \) | sed 's!$(outdir)/!!g' | xargs $(buildscript-dir)/mass-link hard $(outdir) $(top-build-dir)/Documentation/$(outdir)
	touch $@

$(outdir)/lilypond-%.info: $(outdir)/%.texi $(outdir)/$(INFO_IMAGES_DIR).info-images-dir-dep $(outdir)/version.itexi $(outdir)/weblinks.itexi
	$(MAKEINFO) -I$(src-dir) -I$(outdir) --output=$@ $<

$(outdir)/index.$(ISOLANG).html: TEXI2HTML_INIT = $(WEB_TEXI2HTML_INIT)
$(outdir)/index.$(ISOLANG).html: TEXI2HTML_SPLIT = $(WEB_TEXI2HTML_SPLIT)

$(outdir)/index.$(ISOLANG).html:
	DEPTH=$(depth) $(TEXI2HTML) $(TEXI2HTML_FLAGS) $(TEXI2HTML_SPLIT) --output=$(outdir)/ web.texi
	find $(outdir)/ -name '*.html' | xargs grep -L 'UNTRANSLATED NODE: IGNORE ME' | sed 's!$(outdir)/!!g' | xargs $(buildscript-dir)/mass-link --prepend-suffix .$(ISOLANG) hard $(outdir) $(top-build-dir)/Documentation/$(outdir)

.SECONDARY:
