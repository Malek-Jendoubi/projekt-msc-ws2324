\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{unirostock}
\LoadClass[11pt, a4paper, twoside]{book}
\RequirePackage[utf8]{inputenc}

% Sprachvariablen
\DeclareOption{de}{
    \newcommand{\unirostocklanguage}{ngerman}
    \newcommand{\unirostocktopicheading}{zum Thema}
    \newcommand{\unirostockprintednotice}{Gedruckt auf säurefreiem und elementar-chlorfrei gebleichtem Papier.}
    \newcommand{\unirostocklastchange}{Stand:}
    \newcommand{\unirostockdegreecourse}{Studiengang}
    \newcommand{\unirostockhandedinby}{Vorgelegt von}
    \newcommand{\unirostockenrolmentnumber}{Matrikelnummer}
    \newcommand{\unirostockworkperiod}{Bearbeitungszeitraum}
    \newcommand{\unirostocksupervisor}{Betreuer}
    \newcommand{\unirostockprimaryreviewer}{Erstgutachter}
    \newcommand{\unirostocksecondaryreviewer}{Zweitgutachter}
}
\DeclareOption{en}{
    \newcommand{\unirostocklanguage}{english}
    \newcommand{\unirostocktopicheading}{on the subject of}
    \newcommand{\unirostockprintednotice}{Printed on acid-free and elemental chlorine-free bleached paper.}
    \newcommand{\unirostocklastchange}{As of:}
    \newcommand{\unirostockdegreecourse}{Degree course}
    \newcommand{\unirostockhandedinby}{Handed in by}
    \newcommand{\unirostockenrolmentnumber}{Enrolment number}
    \newcommand{\unirostockworkperiod}{Work period}
    \newcommand{\unirostocksupervisor}{Supervisor}
    \newcommand{\unirostockprimaryreviewer}{Primary Reviewer}
    \newcommand{\unirostocksecondaryreviewer}{Secondary Reviewer}
}

\DeclareOption{darkstyle}{
    \newcommand{\primarycolor}{white}
    \newcommand{\accentcolor}{white}
    \newcommand{\thetitelseite}[4]{\titelseiteneu{#1}{#2}{#3}{#4}}
}
\DeclareOption{lightstyle}{
    \newcommand{\primarycolor}{black}
    \newcommand{\accentcolor}{uniblau}
    \newcommand{\thetitelseite}[4]{\titelseite{#1}{#2}{#3}{#4}}
}
\ProcessOptions\relax

\newcommand*{\license}[1]{\gdef\@license{#1}}
\newcommand*{\@license}{creative-commons}

\newcommand*{\type}[1]{\gdef\@type{#1}}
\newcommand*{\@type}{Masterarbeit}

\newcommand*{\course}[1]{\gdef\@course{#1}}
\newcommand*{\enrolmentnumber}[1]{\gdef\@enrolmentnumber{#1}}
\newcommand*{\supervisor}[1]{\gdef\@supervisor{#1}}
\newcommand*{\primaryreviewer}[1]{\gdef\@primaryreviewer{#1}}
\newcommand*{\workperiod}[1]{\gdef\@workperiod{#1}}
\newcommand*{\faculty}[1]{\gdef\@faculty{#1}}
\newcommand*{\institute}[1]{\gdef\@institute{#1}}
\newcommand*{\workinggroup}[1]{\gdef\@workinggroup{#1}}

\newcommand*{\secondaryreviewer}[1]{\gdef\@secondaryreviewer{#1}}
\newcommand*{\@secondaryreviewer}{}

\RequirePackage[ngerman=ngerman-x-latest]{hyphsubst}
\sloppy

\newcommand*{\makelicense}{%
\clearpage
\thispagestyle{plain}
    ~ \vfill
    \begin{flushright}
    	\normalsize \noindent
    	\ifnum\pdf@strcmp{\@license}{creative-commons}=0 %
            {\Huge \href{http://creativecommons.org/licenses/by-sa/4.0/}{\ccbysa}} \\*[0.5em]
            \ifnum\pdf@strcmp{\unirostocklanguage}{ngerman}=0 %
    	        Dieses Werk ist lizenziert unter einer \href{http://creativecommons.org/licenses/by-sa/4.0/}{Creative Commons Namensnennung 4.0 International Lizenz}.
    	    \else
    	        This work is licensed under a \href{http://creativecommons.org/licenses/by-sa/4.0/}{Creative Commons Attribution 4.0 International License}.
    	    \fi
    	    \\
        \else
             {\Huge \copyright}
            \ifnum\pdf@strcmp{\unirostocklanguage}{ngerman}=0 %
                Alle Inhalte dieses Werkes, insbesondere Texte und Grafiken, sind urheberrechtlich geschützt. Das Urheberrecht liegt, soweit nicht ausdrücklich anders gekennzeichnet, bei \@author.
            \else
                All contents of this work, in particular texts and graphics, are protected by copyright. Unless explicitly stated otherwise, the copyright belongs to \@author.
            \fi
        \fi
    	
    	\vspace{3.5cm}
    	\tiny{}	\unirostockprintednotice\\
        \rule{1.90cm}{0.4pt}\\
    	\unirostocklastchange: \today
    \end{flushright}
    \clearpage
    \restoregeometry
}

\renewcommand*{\maketitle}{%
\newgeometry{top=2.5cm,bottom=2.0cm,left=1.5cm,right=2.5cm}
\begin{titlepage}
    \thetitelseite{\@workinggroup}
                  {\@institute}
                  {\@faculty}
                  {Universität Rostock}
    \vspace*{4cm}
    \Atitel{\@type}{\@title}
    
    \vspace{5cm}
    
    \textsf{\color{\primarycolor}
    \large\centering\doublespacing
    \hspace{1.3cm}\begin{tabular}{p{4.0cm}p{0.3cm} p{8.7cm}l}
    \unirostockdegreecourse: & & \@course\\
    \unirostockhandedinby: & & \@author\\
    \unirostockenrolmentnumber: & & \@enrolmentnumber \\
    \unirostockworkperiod: & & \@workperiod\\
    \unirostocksupervisor: & & \@supervisor\\
    \unirostockprimaryreviewer: & & \@primaryreviewer\\
    \ifnum\pdf@strcmp{\@secondaryreviewer}{}>0 %
    \unirostocksecondaryreviewer: & & \@secondaryreviewer\\
    \fi
    \end{tabular}
    }
\end{titlepage}

\setlength{\parindent}{0cm}
}

\RequirePackage[\unirostocklanguage]{babel} 
\RequirePackage{graphicx}
\RequirePackage{color}
\RequirePackage{textcomp}
\RequirePackage{xcolor}
\definecolor{uniblau}{HTML}{004a99}
\definecolor{uni}{HTML}{004a99}
\definecolor{hellblau}{HTML}{087FFB}
\RequirePackage{setspace}
\RequirePackage{tocloft}
\RequirePackage{listings}
\RequirePackage{ccicons}
\RequirePackage{enumitem}
\RequirePackage{Alegreya}
\RequirePackage{AlegreyaSans}

\renewcommand{\cftloftitlefont}{\color{uniblau}\huge\itshape\sffamily}
\renewcommand{\cftlottitlefont}{\color{uniblau}\huge\itshape\sffamily}
\renewcommand{\cfttoctitlefont}{\color{uniblau}\Huge\itshape\sffamily}


\usepackage[top=2.5cm,bottom=2.7cm,footskip=1.7cm, headsep=1cm, 
% PDF-Version:
left=3.3cm,right=3.3cm,
% Druckversion:
%bindingoffset=8mm, twoside, left=3.3cm,right=3.3cm,
]{geometry}

\usepackage[ % Paket zur Erstellung von Links in der PDF-Datei.
   pdftoolbar=false,
   pdfmenubar=false,
]{hyperref}

\hypersetup{
    colorlinks=false,
    urlbordercolor=uniblau,
    linkbordercolor=uniblau,
    pdfborder={1 1 1},
    citebordercolor=hellblau,
    citecolor=,
    pdfauthor=\@author,
    pdftitle=\@title,
    pdfpagemode=FullScreen
    }
%%%

\RequirePackage{tikz}
\usetikzlibrary{shadows.blur}

\RequirePackage[\unirostocklanguage]{minitoc}
\RequirePackage{calc}

\renewcommand{\mtifont}{\large\sffamily}
\renewcommand{\mtcfont}{\small\sffamily}
\renewcommand{\mtcSfont}{\small\sffamily}
\renewcommand{\mtcSSfont}{\small\sffamily}
\renewcommand{\mtcSSSfont}{\small\sffamily}

\RequirePackage[]{titlesec}

\let\oldl@chapter\l@chapter
\def\l@chapter#1#2{\oldl@chapter{#1}{\textcolor{black}{#2}}}

\let\old@dottedcontentsline\@dottedtocline
\def\@dottedtocline#1#2#3#4#5{%
\old@dottedcontentsline{#1}{#2}{#3}{#4}{{\textcolor{black}{#5}}}}

\titleformat{\chapter}[display]
  {\normalfont\color{uniblau}}
  {\filleft\Huge\sffamily\bfseries\chaptertitlename\hspace*{2mm}%
  \begin{tikzpicture}[baseline={([yshift=-.6ex]current bounding box.center)}]
    \node[fill=uniblau,circle,text=white] {\thechapter};
  \end{tikzpicture}}
  {1ex}
  {\titlerule[1.5pt]\vspace*{5ex}\huge\sffamily\itshape}
  []

\titleformat{name=\chapter,numberless}[display]
  {\normalfont\color{uniblau}}
  {}
  {1ex}
  {\vspace*{5ex}\huge\sffamily\itshape}
  []

\setcounter{minitocdepth}{1}

\addto{\captionsngerman}{% Making babel aware of special titles
  \renewcommand{\mtctitle}{In diesem Kapitel}
  \renewcommand{\contentsname}{Inhalt}
  \renewcommand{\listfigurename}{Abbildungen}
  \renewcommand{\listtablename}{Tabellen}
}

\addto{\captionsenglish}{% Making babel aware of special titles
  \renewcommand{\mtctitle}{In this chapter}
  \renewcommand{\contentsname}{Contents}
  \renewcommand{\listfigurename}{Figures}
  \renewcommand{\listtablename}{Tables}

}

%command to print the acutal minitoc
\newcommand{\printmyminitoc}[1]{%
    \vspace{-2cm}%
    \noindent\hspace{-4.5cm}%
    \colorlet{black}{white}%
    \begin{tikzpicture}
    \node[rounded corners,align=left,fill=uniblau, blur shadow={shadow blur steps=5}, inner sep=5mm]{%
        \hspace{3.8cm}
        \color{white}%
        \begin{minipage}{8.5cm}%minipage trick
        
        \hypersetup{linkcolor=white}
        \minitoc
        \hypersetup{linkcolor=uniblau}
        \end{minipage}};
    \end{tikzpicture}
    \vspace{1cm}%
}

\onehalfspacing % Zeilenabstand ist anderthalbzeilig.

\usepackage{fancyhdr} % Paket zur Erstellung von Kopfzeilen.
\setlength{\headheight}{13.6pt}
\fancypagestyle{plain}{%
\fancyhf{}}
\renewcommand{\headrulewidth}{0pt} % Obere Linie.
\renewcommand{\footrulewidth}{0pt} % Untere Linie.
	\pagestyle{fancy}

%	\fancyhead{}			
%	\fancyfoot{}
	\lhead{\leftmark} % Text oben links.
	\rhead{} % Text oben rechts.

% Befehl zum Setzen von Logo und Rahmen. Zwei Variablen für den Text in der unteren Box.
\newcommand{\titelseite}[4]{
\begin{tikzpicture}[overlay]
\draw [rounded corners=1.2cm, line width=1.5pt, color=uniblau] (-5.2,-4) rectangle (17, -28);
\node [right] at (12.7, 0) {\includegraphics[width=3cm]{images/800600.pdf}};
\node [right] at (-1.8, -1) {\includegraphics[width=9cm]{images/unihro.pdf}};
\draw [fill=uni, thick, line width=1.5pt, color=uni] (-5.2,-28) rectangle (17, -23.5);
\node [right, white] at (0,-24.5) {\large{\textsf{#1}}};
\node [right, white] at (0,-25) {\large{\textsf{#2}}};
\node [right, white] at (0,-25.5) {\large{\textsf{#3}}};
\node [right, white] at (0,-26) {\large{\textsf{#4}}};
\node [right] at (12.7, -25.7) {\includegraphics[width=1cm,natwidth=44.5mm,natheight=27.5mm]{images/lehrstuhllogo.png}};
\end{tikzpicture}
}

\newcommand{\titelseiteneu}[4]{
\begin{tikzpicture}[overlay]
\node [right] at (-2.26, -11.98) {\includegraphics[width=1.01\paperwidth]{images/Hintergrund_Titel.pdf}};
\node [right, white] at (0,-24.5) {\large{\textsf{#1}}};
\node [right, white] at (0,-25) {\large{\textsf{#2}}};
\node [right, white] at (0,-25.5) {\large{\textsf{#3}}};
\node [right, white] at (0,-26) {\large{\textsf{#4}}};
\node [right] at (12.7, -25.7) {\includegraphics[width=1cm,natwidth=44.5mm,natheight=27.5mm]{images/lehrstuhllogo.png}};
\end{tikzpicture}
}

\newcommand{\Atitel}[2]{
\color{\primarycolor}
\begin{center}
\setstretch{1.9213204424}
\textsf{\textbf{{#1} \unirostocktopicheading}}\\[1cm]
\textcolor{\accentcolor}{\huge{\textbf{\textsf{#2}}}}
\end{center}
}

\usepackage{csquotes}

\newcommand*{\digsigfield}[3]{\hbox{%
  \pdfstringdef\Fld@name{#3}%
  \def\Fld@align{1}%	Is this sensible? Do we need an option?
  \Fld@hiddenfalse
  \dimen255=#1\relax\ifdim\dimen255 < 2sp
    \dimen255=#2\relax\ifdim\dimen255 < 2sp \Fld@hiddentrue \fi\fi
    % No cheating with negative values! :-)
  \setbox0=\hbox{\rule{0mm}{#2}\kern#1}%
  % There has been an unfortunate (and in my opinion ill-considered) change
  % between pdftex 1.10b and pdfetex 1.21a: while \pdfstartlink/\pdfendlink
  % could formerly be called if \pdfoutput was zero, this is no longer
  % permitted in the newer versions. Hence the following is needed twice:
  \def\helper{\expandafter\ifx\csname pdfmark\endcsname\relax
      \box0\PackageWarningNoLine{digsig}{No pdfmark support in selected
	hyperref driver, hence no signature field ``\Fld@name''}%
    \else
      \pdfmark[\box0]{pdfmark=/ANN,Raw={\PDFFormExtension@Signature}}%
    \fi
  }%
  \expandafter\ifx\csname pdfoutput\endcsname\relax
    \helper
  \else\ifnum\pdfoutput > 0
    \pdfstartlink user {\PDFFormExtension@Signature}\box0\pdfendlink
  \else
    \helper
  \fi\fi}}

% Internal macro, similar to the \PDFForm@* macros of hyperref. The omission of
% parameterization for "MK" is deliberate: if you need a border, create it with
% TeX.
\newcommand{\PDFFormExtension@Signature}{%
  /T (\Fld@name)
  /Subtype /Widget
  /FT /Sig
  /F \ifFld@hidden 2\else 4\fi\space
  /Q \Fld@align\space
  /MK << /BC [] >>}
\setlength{\parskip}{0.5cm plus0.25cm minus0.25cm} % Abstand zwischen zwei Absätzen
\dominitoc