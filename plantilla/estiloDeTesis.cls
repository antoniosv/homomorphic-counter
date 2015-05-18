\NeedsTeXFormat{LaTeX2e}[1995/12/01]
\ProvidesClass{fime}
\LoadClass[12pt,oneside]{book}
%\usepackage[spanish]{babel}
\usepackage[numbers,sort]{natbib}
\usepackage{multirow}
\usepackage[ansinew]{inputenc}
\usepackage{amsmath,amsfonts,amssymb}
\usepackage[pdftex]{graphicx}
\usepackage{epstopdf}
\usepackage[clearempty,pagestyles]{titlesec}
%\usepackage{subfigure}
\usepackage[intoc]{nomencl}
\newtheorem{teorema}{Teorema}[chapter]
\newtheorem{algoritmo}[teorema]{Algoritmo}
\newtheorem{definicion}[teorema]{Definición}
\newtheorem{ejemplo}[teorema]{Ejemplo}
\voffset = -2.0cm
\topmargin = 0.6cm
\headheight = 0.6cm
\headsep = 0.66cm
\textheight = 22.94cm
\hoffset = 0.0cm
\oddsidemargin = 1.5cm
\textwidth = 15.05cm
\titleformat{\chapter}[display]{\large \scshape}
	{\filcenter \chaptertitlename\ \thechapter}{-0.75em}
	{\filcenter \Huge}[\titlerule\vspace{-0.4em}]
\titleformat{\section}[hang]{\vspace{-0.4em}\Large \scshape}
	{}{0mm}{\filright \thesection\ }[\vspace{-0.2em}]
\titleformat{\subsection}[hang]{\vspace{-0.4em}\large \scshape}
	{}{0mm}{\filright \thesubsection\ }[\vspace{-0.2em}]
\titleformat{\subsubsection}[hang]{\vspace{-0.4em}\scshape}{}{0mm}{\filright}
\titleformat{\paragraph}[runin]{\vspace{-0.4em}\scshape}{}{0mm}{}
\titleformat{\subparagraph}[runin]{\vspace{-0.4em}\scshape}{}{0mm}{}
\newpagestyle{main}[\scshape]{
	\sethead{\chaptertitle}{}{\thepage}
	\headrule}
\newpagestyle{nom}[\scshape]{
	\sethead{Nomenclature}{}{\thepage}
	\headrule}
\newpagestyle{fime}[\scshape]{
	\sethead{\chaptertitlename\ \thechapter. \chaptertitle}{}{\thepage}
	\headrule}
\parskip = 0.75em
\parindent = 10mm
\def\baselinestretch{1.5}
