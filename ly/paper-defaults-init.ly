%%%% This file is part of LilyPond, the GNU music typesetter.
%%%%
%%%% Copyright (C) 2004--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>
%%%%                          Jan Nieuwenhuizen <janneke@gnu.org>
%%%%                          Neil Puttock <n.puttock@gmail.com>
%%%%
%%%% LilyPond is free software: you can redistribute it and/or modify
%%%% it under the terms of the GNU General Public License as published by
%%%% the Free Software Foundation, either version 3 of the License, or
%%%% (at your option) any later version.
%%%%
%%%% LilyPond is distributed in the hope that it will be useful,
%%%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%%%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%%%% GNU General Public License for more details.
%%%%
%%%% You should have received a copy of the GNU General Public License
%%%% along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.

\version "2.12.0"

\paper {
  %%% WARNING
  %%%
  %%% If you add any new dimensions, don't forget to update
  %%% the dimension-variables variable.  See paper.scm.

  unit = #(ly:unit)
  mm = 1.0
  in = 25.4
  pt = #(/ in 72.27)
  cm = #(* 10 mm)

  print-page-number = ##t

  %%
  %% 20pt staff, 5 pt = 1.75 mm
  %%

  output-scale = #1.7573

  #(define-public book-title (marked-up-title 'bookTitleMarkup))
  #(define-public score-title (marked-up-title 'scoreTitleMarkup))

  %%
  %% ugh. hard coded?
  %%

  #(layout-set-absolute-staff-size (* 20.0 pt))


  #(define-public score-title-properties
     '((is-title . #t)
       (is-book-title . #f)))
  #(define-public book-title-properties
     '((is-title . #t)
       (is-book-title . #t)))

  %% Note: these are not scaled; they are in staff-spaces.
  between-system-spacing = #'((space . 12) (minimum-distance . 8) (padding . 1))
  between-scores-system-spacing = #'((space . 14) (minimum-distance . 8) (padding . 1))
  after-title-spacing = #'((space . 2) (padding . 0.5))
  before-title-spacing = #'((space . 5) (padding . 0.5))
  between-title-spacing = #'((space . 1) (padding . 0.5))
  top-system-spacing = #'((space . 1) (padding . 1) (minimum-distance . 0))
  top-title-spacing = #'((space . 0) (padding . 1) (minimum-distance . 0))
  bottom-system-spacing = #'((space . 1) (padding . 1) (minimum-distance . 0) (stretchability . 5))

  ragged-bottom = ##f

  %%
  %% looks best for shorter scores.
  %%
  ragged-last-bottom = ##t

  %%
  %% settings for the page breaker
  %%
  blank-last-page-force = 0
  blank-after-score-page-force = 2
  blank-page-force = 5

  #(define font-defaults
    '((font-family . feta) (font-encoding . fetaMusic)))

  %%
  %% the font encoding `latin1' is a dummy value for Pango fonts
  %%
  #(define text-font-defaults
     `((font-encoding . latin1)
       (baseline-skip . 3)
       (word-space . 0.6)))

  #(define page-breaking ly:optimal-breaking)

  #(define make-header (marked-up-headfoot 'oddHeaderMarkup 'evenHeaderMarkup))
  #(define make-footer (marked-up-headfoot 'oddFooterMarkup 'evenFooterMarkup))
  #(set-paper-dimension-variables (current-module))

  \include "titling-init.ly"

  check-consistency = ##t
  two-sided = ##f

  % These margins apply to the default paper format given by (ly:get-option 'paper-size)
  % and are scaled accordingly for other formats

  top-margin-default = 5 \mm
  bottom-margin-default = 6 \mm

  left-margin-default = 10 \mm
  right-margin-default = 10 \mm

  inner-margin-default = 10 \mm
  outer-margin-default = 20 \mm
  binding-offset-default = 0 \mm

  head-separation-default = 4 \mm
  foot-separation-default = 4 \mm

  indent-default = 15 \mm
  short-indent-default = 0 \mm

  first-page-number = #1
  print-first-page-number = ##f
}
