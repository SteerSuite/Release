
The most recent documentation can be found on the SteerSuite web page.
If you want to build this version of documentation anyway, follow
these three steps:

1. Make sure you have the following free downloadable utilities:

  - docbook xsl stylesheets:  These define how the xml input should be
    converted into html or fo format.
    http://docbook.sourceforge.net/

  - fop: The Apache tool for converting .fo files into .pdf files.  This
    is only required if you wish to build the pdf documentation.
    http://xmlgraphics.apache.org/

  - XSLT library: The library and tool (xsltproc) for "rendering" the
    docbook XML input into nicely formatted html or an intermediate .fo
    format.  XSLT is already provided on most systems.
    http://xmlsoft.org/XSLT/

  - doxygen: A tool for creating documentation out of code comments.
    This is only required if you wish to build the code reference.
    You may need a recent version of doxygen, such as 1.5.8.
    http://www.doxygen.org/


2. Edit some varibles in the makefile to refer to the correct file paths.

  "DOCBOOK_XSL_PATH" - should point to the root directory of your docbook
                   installation.

  "FOP" - should point to the fop executable script on your system.

  "XSLTPROC" - should point to the xsltproc executable on your system.

  "DOXYGEN" - should point to the doxygen executable on your system.


3. Run any of the following commands:

  "make all"     - the same as "make single" and "make doxygen" combined.

  "make single"  - makes single-page html documentation for 
                   the User Guide and Reference Manual.

  "make multi"   - makes multi-page html documentation for
                   the User Guide and Reference Manual.

  "make pdf"     - makes the User Guide and Reference Manual in pdf 
                   format.

  "make doxygen" - builds the doxygen code reference in html format.


The output of these commands will be found in the
UserGuide/, ReferenceManual/, and CodeReference/ directories.


