\version "2.11.15"
%% +.ly: Be the first .ly file for lys-to-tely.py.
%% Better to make lys-to-tely.py include "introduction.texi" or
%% other .texi documents too?

\header{
texidoc = #(string-append "
@unnumbered Introduction

This document shows examples from the
@uref{http://lsr@/.dsi@/.unimi@/.it,LilyPond Snippet Repository}.

In the web version of this document, you can click on the file name
or figure for each example to see the corresponding input file.

This document is for LilyPond version 
" (lilypond-version) "." )
}

% make sure .png  is generated.
\lyrics {  " " }