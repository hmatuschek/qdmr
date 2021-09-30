IMG = fm_simplex_a.png fm_simplex_b.png fm_duplex_a.png fm_duplex_b.png fm_echolink_a.png fm_echolink_b.png fm_echolink_c.png

#.PRECIOUS: %.pdf # keep PDFs
all: $(IMG)

%.png: %.pdf
	convert -density 300 -depth 8 -quality 85 $< $@

%.pdf: %.tex
	pdflatex $<

clean:
	rm -f $(IMG)
