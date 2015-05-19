#B1;2802;0call: tesis.pdf

tesis.pdf: tesis.tex bibliografia.bib portillas.tex resumen.tex
	pdflatex --enable-write18 --shell-scape tesis
	bibtex tesis	
	makeindex tesis
	touch tesis.bbl	
	rm tesis.bbl
	touch tesis.bbl.bak
	cp tesis.bbl.bak tesis.bbl
	pdflatex --enable-write18 --shell-scape tesis
	pdflatex --enable-write18 --shell-scape tesis


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
	-rm tesis.pdf
