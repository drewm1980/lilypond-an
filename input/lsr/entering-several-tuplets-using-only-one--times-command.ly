%% Do not edit this file; it is auto-generated from LSR http://lsr.dsi.unimi.it
%% This file is in the public domain.
\version "2.11.38"

\header {
  lsrtags = "rhythms"
 texidoc = "
This example shows how to specify how long each of the tuplets
contained within the bracket after @code{\\times} should last.  Many
consecutive tuplets can then be contained within a single @code{\\times
@{ ... @}}, thus saving typing.

In the example, two triplets are shown, while @code{\\times} was
entered only once.


For more information about @code{make-moment}, see \"Time
administration\". 
" }
% begin verbatim
\relative {
  \set tupletSpannerDuration = #(ly:make-moment 1 4)
  \times 2/3 { c8 c c c c c }
}