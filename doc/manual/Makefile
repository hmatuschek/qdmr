SRC = manual.xml $(shell find meta/ *.*) $(shell find intro/ *.*) $(shell find gui/ *.*) $(shell find codeplug/ *.*) $(shell find conf/ *.*) $(shell find cli/ *.*) 

all: images manual.pdf manual.epub html 

images:
	make -C intro/fig
	make -C gui/fig
	

manual.pdf: manual.fo
	fop -fo manual.fo -pdf manual.pdf
manual.fo: manual_combined.xml manual_fo.debian.xsl
	xsltproc -o manual.fo manual_fo.debian.xsl manual_combined.xml
manual_combined.xml: $(SRC) 
	xmllint --nonet --noent --xinclude manual.xml > manual_combined.xml


manual.epub: images-epub manual_combined.xml 
	cp manual_combined.xml epub/manual.xml 
	make -C epub 
images-epub: images epub/fig
	install intro/fig/*.png epub/fig 
	install gui/fig/*.png epub/fig 
epub/fig:
	mkdir epub/fig 


html: images-html
	cp manual_combined.xml html/manual.xml 
	make -C html 
images-html: images html/fig manual_combined.xml
	install intro/fig/*.png html/fig 
	install gui/fig/*.png html/fig 
html/fig:
	mkdir html/fig


clean: 
	make -C intro/fig clean
	make -C epub clean
	make -C html clean
	rm -f manual.pdf manual.fo manual_combined.xml
