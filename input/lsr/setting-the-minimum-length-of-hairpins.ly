%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.46"

\header {
  lsrtags = "expressive-marks"

  texidoc = "
If hairpins are too short, they can be lengthened by modifying the
@code{minimum-length} property of the @code{Hairpin} object. 

"
  doctitle = "Setting the minimum length of hairpins"
} % begin verbatim
\relative c'' {
  c4\< c\! d\> e\!
  \override Hairpin #'minimum-length = #5
  << f1 { s4 s\< s\> s\! } >>
}