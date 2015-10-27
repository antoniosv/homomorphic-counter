ALL: poster.pdf 

%.pdf: %.tex Makefile
	pdflatex $< &&	pdflatex $< && pdflatex $<

clean:
	  rm -f *.aux	*.bbl	*.blg	*.log	poster.pdf
