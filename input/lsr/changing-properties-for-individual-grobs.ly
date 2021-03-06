%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.64"

\header {
  lsrtags = "tweaks-and-overrides"

  texidoc = "
The @code{\\applyOutput} command allows the tuning of any layout
object, in any context. It requires a Scheme function with three
arguments.

"
  doctitle = "Changing properties for individual grobs"
} % begin verbatim

\layout {
  ragged-right = ##t
}

#(define (mc-squared grob grob-origin context)
  (let*
   (
     (ifs (ly:grob-interfaces grob))
     (sp (ly:grob-property grob 'staff-position))
   )
   (if (memq 'note-head-interface ifs)
    (begin
     (ly:grob-set-property! grob 'stencil ly:text-interface::print)
     (ly:grob-set-property! grob 'font-family 'roman)
     (ly:grob-set-property! grob 'text
      (make-raise-markup -0.5
       (case sp
	((-5) (make-simple-markup "m"))
	((-3) (make-simple-markup "c "))
	((-2) (make-smaller-markup (make-bold-markup "2")))
	(else (make-simple-markup "bla"))
      ))))
  )))

\relative c' {
  <d f g b>2
  \applyOutput #'Voice #mc-squared
  <d f g b>2
}
