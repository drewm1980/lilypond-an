default:

local-WWW-2: $(OUT_HTML_FILES)
	$(PYTHON) $(buildscript-dir)/mass-link.py --prepend-suffix .$(ISOLANG) hard $(outdir) $(top-build-dir)/Documentation/$(outdir) $(HTML_FILES)
