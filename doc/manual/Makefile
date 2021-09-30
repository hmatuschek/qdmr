SRC = manual.xml $(shell find intro/ *.*) $(shell find gui/ *.*) $(shell find codeplug/ *.*) $(shell find conf/ *.*)

all: manual.pdf

manual.pdf: manual.fo
	fop -fo manual.fo -pdf manual.pdf

manual.fo: manual_combined.xml manual_fo.debian.xsl
	xsltproc -o manual.fo manual_fo.debian.xsl manual_combined.xml

manual_combined.xml: $(SRC) 
	xmllint --nonet --noent --xinclude manual.xml > manual_combined.xml
	
clean: 
	rm -f manual.pdf manual.fo manual_combined.xml
