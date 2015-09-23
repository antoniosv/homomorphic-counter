all: presentation

presentation: presentation.tex
	pdflatex presentation.tex
	bibtex presentation	
	pdflatex presentation.tex
	pdflatex presentation.tex

clean:
	-rm *~
	-rm *.aux
	-rm *.log
	-rm *.out
	-rm *.toc
	-rm *.bbl
	-rm *.blg
	-rm *.glo
	-rm *.idx
	-rm *.ilg
	-rm *.ind
	-rm *.lof
	-rm *.lot
	-rm *.nlo
	-rm presentation.pdf
