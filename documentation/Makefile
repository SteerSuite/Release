#
# Copyright (c) 2009 Shawn Singh, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
# See license.txt for complete license.
#

#
# Most likely you will need to edit the first 3 variables below
# to point to the appropriate files/directories.
#

# The path where the docbook stylesheets can be found.
# Optionally (not recommended), use the http path to use the
# stylesheets directly from the internet - this will be very slow!
DOCBOOK_XSL_PATH = /usr/share/docbook-xsl
#DOCBOOK_XSL_PATH = http://docbook.sourceforge.net/release/xsl/current

XSLTPROC = /usr/bin/xsltproc

# The path where doxygen binary can be found.  Recommended at least
# version 1.5.8, which was used for the .cfg file.
DOXYGEN = ~/bin/doxygen

# The path to the Apache fop utility.  Only needed if you want to 
# make pdf documentation.
FOP = ~/bin/fop



###############################################
# Probably do not need to modify below this.
###############################################

INPUT_DIR = rawinput/

XSLTPROC_COMMON_OPTIONS = --stringparam generate.index 1 --stringparam admon.graphics 1 --stringparam admon.graphics.extension ".png" --stringparam html.stylesheet "docbook-html-style.css" --stringparam section.autolabel 1 --stringparam section.autolabel.max.depth 1 --stringparam section.label.includes.component.label 1

SINGLE_XSLTPROC_OPTIONS = $(XSLTPROC_COMMON_OPTIONS) --stringparam root.filename index --stringparam chunk.quietly 1
MULTI_XSLTPROC_OPTIONS =  $(SINGLE_XSLTPROC_OPTIONS) --stringparam use.id.as.filename 1 --stringparam chunk.section.depth 0
PDF_XSLTPROC_OPTIONS = --output temp.fo $(XSLTPROC_COMMON_OPTIONS) --stringparam body.start.indent 0pt --stringparam shade.verbatim 1



all: single doxygen

.PHONY: all clean single multi pdf doxygen

# the cygwin xsltproc command seems to ignore "-o" command and always default output to index.html.
# so, we use "mv" instead to place the file where we want.
single: UserGuide ReferenceManual
	@echo -n Making single-page html User Guide...
	@$(XSLTPROC) $(SINGLE_XSLTPROC_OPTIONS) $(DOCBOOK_XSL_PATH)/xhtml/onechunk.xsl $(INPUT_DIR)SteerSuiteUserGuide.xml
	@cp $(INPUT_DIR)docbook-html-style.css UserGuide/docbook-html-style.css
	@cp $(DOCBOOK_XSL_PATH)/images/*.png UserGuide/images
	@mv index.html UserGuide/SteerSuiteUserGuide.html
	@echo done.
	@echo -n Making single-page html Reference Manual...
	@$(XSLTPROC) $(SINGLE_XSLTPROC_OPTIONS) $(DOCBOOK_XSL_PATH)/xhtml/onechunk.xsl $(INPUT_DIR)SteerSuiteReferenceManual.xml
	@cp $(INPUT_DIR)docbook-html-style.css ReferenceManual/docbook-html-style.css
	@cp $(DOCBOOK_XSL_PATH)/images/*.png ReferenceManual/images
	@mv index.html ReferenceManual/SteerSuiteReferenceManual.html
	@echo done.


multi: UserGuide ReferenceManual
	@echo -n Making multi-page html User Guide...
	@$(XSLTPROC) $(MULTI_XSLTPROC_OPTIONS) $(DOCBOOK_XSL_PATH)/xhtml/chunk.xsl $(INPUT_DIR)SteerSuiteUserGuide.xml
	@cp $(INPUT_DIR)docbook-html-style.css UserGuide/docbook-html-style.css
	@cp $(DOCBOOK_XSL_PATH)/images/*.png UserGuide/images
	@mv *.html UserGuide/
	@echo done.
	@echo -n Making multi-page html Reference Manual...
	@$(XSLTPROC) $(MULTI_XSLTPROC_OPTIONS) $(DOCBOOK_XSL_PATH)/xhtml/chunk.xsl $(INPUT_DIR)SteerSuiteReferenceManual.xml
	@cp $(INPUT_DIR)docbook-html-style.css ReferenceManual/docbook-html-style.css
	@cp $(DOCBOOK_XSL_PATH)/images/*.png ReferenceManual/images
	@mv *.html ReferenceManual/
	@echo done.


pdf: UserGuide ReferenceManual
	@echo -n Making pdf User Guide...
	@$(XSLTPROC) $(PDF_XSLTPROC_OPTIONS) $(DOCBOOK_XSL_PATH)/fo/docbook.xsl $(INPUT_DIR)SteerSuiteUserGuide.xml
	@$(FOP) temp.fo UserGuide/SteerSuiteUserGuide.pdf
	@rm -f temp.fo
	@echo done.
	@echo -n Making pdf Reference Manual...
	@$(XSLTPROC) $(PDF_XSLTPROC_OPTIONS) $(DOCBOOK_XSL_PATH)/fo/docbook.xsl $(INPUT_DIR)SteerSuiteReferenceManual.xml
	@$(FOP) temp.fo ReferenceManual/SteerSuiteReferenceManual.pdf
	@rm -f temp.fo
	@echo done.


doxygen: CodeReference
	@echo -n Making doxygen code reference...
	@$(DOXYGEN) $(INPUT_DIR)doxygen-1.5.8.cfg
	@echo done.


clean:
	@echo -n Cleaning up...
	@rm -rf UserGuide ReferenceManual CodeReference
	@rm -rf *.html *.fo doxygen-warnings.log
	@echo done.


# the following directories are created if they do not exist.

UserGuide:
	@mkdir -p UserGuide
	@mkdir -p UserGuide/images

ReferenceManual:
	@mkdir -p ReferenceManual
	@mkdir -p ReferenceManual/images

CodeReference:
	@mkdir -p CodeReference

