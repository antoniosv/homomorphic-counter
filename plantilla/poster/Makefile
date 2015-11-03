ALL: poster.pdf 

%.pdf: %.tex Makefile
	pdflatex --enable-write18 -shell-escape $< &&	pdflatex $< && pdflatex $<


clean:
	  rm -f *.aux	*.bbl	*.blg	*.log	poster.pdf
