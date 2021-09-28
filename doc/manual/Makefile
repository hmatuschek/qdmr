SRC = manual.xml preface.xml introduction.xml gui.xml extensible_codeplug.xml \
      radioddity_extensions.xml opengd77_extensions.xml tyt_extensions.xml \
			anytone_extensions.xml

all: manual.pdf

manual.pdf: manual.fo
	fop -fo manual.fo -pdf manual.pdf

manual.fo: manual_combined.xml
	xsltproc -o manual.fo --stringparam paper.type A4 /usr/share/xml/docbook/stylesheet/docbook-xsl/fo/docbook.xsl manual_combined.xml

manual_combined.xml: ${SRC}
	xmllint --nonet --noent --xinclude manual.xml > manual_combined.xml

clean: 
	rm -f manual.pdf manual.fo
