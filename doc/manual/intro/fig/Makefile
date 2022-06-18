IMG = fm_simplex_a.png fm_simplex_b.png fm_duplex_a.png fm_duplex_b.png fm_echolink_a.png fm_echolink_b.png fm_echolink_c.png \
      trunk_net_ex1.png trunk_net_ex2.png trunk_net_ex3.png trunk_net_ex4a.png trunk_net_ex4b.png \
			simplex_allcall.png simplex_groupcall.png simplex_privatecall.png \
			repeater_local.png repeater_privatecall.png \
			talkgroup_ex1a.png talkgroup_ex1b.png talkgroup_ex1c.png \
			timeslot.png

#.PRECIOUS: %.pdf # keep PDFs
all: $(IMG)

%.png: %.pdf
	convert -density 300 -depth 8 -quality 85 $< $@

%.pdf: %.tex
	pdflatex $<

clean:
	rm -f $(IMG)
	rm -f *.aux *.log
