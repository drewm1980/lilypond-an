TITLE=LYs Doc


ifeq ($(COLLATED_FILES),)
COLLATED_FILES = $(sort $(TEXINFO_SOURCES) $(LY_FILES) $(OUT_LY_FILES) )
endif
