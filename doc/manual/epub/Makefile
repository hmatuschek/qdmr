all: manual.epub 

%.epub: %.xml
	pandoc -f docbook -t epub3 -o $@ $<

%.xml:

clean:
	rm -f manual.epub 
	rm -f manual.xml 
	rm -rf fig/