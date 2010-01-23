;;;; This file is part of LilyPond, the GNU music typesetter.
;;;;
;;;; Copyright (C) 1998--2010  Han-Wen Nienhuys <hanwen@xs4all.nl>
;;;;                 Jan Nieuwenhuizen <janneke@gnu.org>
;;;;
;;;; LilyPond is free software: you can redistribute it and/or modify
;;;; it under the terms of the GNU General Public License as published by
;;;; the Free Software Foundation, either version 3 of the License, or
;;;; (at your option) any later version.
;;;;
;;;; LilyPond is distributed in the hope that it will be useful,
;;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;;; GNU General Public License for more details.
;;;;
;;;; You should have received a copy of the GNU General Public License
;;;; along with LilyPond.  If not, see <http://www.gnu.org/licenses/>.


;; should include default value?


(ly:add-interface
 'accidental-suggestion-interface
 "An accidental, printed as a suggestion (typically: vertically over a
note)."
 '())

(ly:add-interface
 'ambitus-interface
 "The line between note heads for a pitch range."
 '(gap note-heads thickness))

(ly:add-interface
 'bass-figure-interface
 "A bass figure text."
 '(implicit))

(ly:add-interface
 'bass-figure-alignment-interface
 "Align a bass figure."
 '())

(ly:add-interface
 'bend-after-interface
 "A doit or drop."
 '(thickness delta-position))

(ly:add-interface
 'dynamic-interface
 "Any kind of loudness sign."
 '())

(ly:add-interface
 'dynamic-line-spanner-interface
 "Dynamic line spanner."
 '(avoid-slur))

(ly:add-interface
 'dynamic-text-interface
 "An absolute text dynamic."
 '(right-padding))

(ly:add-interface
 'dynamic-text-spanner-interface
 "Dynamic text spanner."
 '(text))

(ly:add-interface
 'finger-interface
 "A fingering instruction."
 '())

(ly:add-interface
 'fret-diagram-interface
 "A fret diagram"
 '(align-dir fret-diagram-details size dot-placement-list
   thickness))

(ly:add-interface
 'grace-spacing-interface
 "Keep track of durations in a run of grace notes."
 '(columns common-shortest-duration))

(ly:add-interface
 'instrument-specific-markup-interface
 "Instrument-specific markup (like fret boards or harp pedal diagrams)."
 '(fret-diagram-details harp-pedal-details size thickness))

(ly:add-interface
 'key-cancellation-interface
 "A key cancellation."
 '())

(ly:add-interface
 'ligature-bracket-interface
 "A bracket indicating a ligature in the original edition."
 '(width thickness height))

(ly:add-interface
 'ligature-interface
 "A ligature."
 '())

(ly:add-interface
 'lyric-interface
 "Any object that is related to lyrics."
 '())

(ly:add-interface
 'lyric-syllable-interface
 "A single piece of lyrics."
 '())

(ly:add-interface
 'mark-interface
 "A rehearsal mark."
 '())

(ly:add-interface
 'metronome-mark-interface
 "A metronome mark."
 '())

(ly:add-interface
 'multi-measure-interface
 "Multi measure rest, and the text or number that is printed over it."
 '(bound-padding))

(ly:add-interface
 'note-name-interface
 "Note names."
 '())

(ly:add-interface
 'only-prebreak-interface
 "Kill this grob after the line breaking process."
 '())

(ly:add-interface
 'parentheses-interface
 "Parentheses for other objects."
 '(padding stencils))

(ly:add-interface
 'percent-repeat-interface
 "Beat, Double and single measure repeats."
 '(dot-negative-kern slash-negative-kern slope thickness))

(ly:add-interface
 'piano-pedal-interface
 "A piano pedal sign."
 '())

(ly:add-interface
 'piano-pedal-script-interface
 "A piano pedal sign, fixed size."
 '())

(ly:add-interface
 'pitched-trill-interface
 "A note head to indicate trill pitches."
 '(accidental-grob))

(ly:add-interface
 'rhythmic-grob-interface
 "Any object with a duration.  Used to determine which grobs are
interesting enough to maintain a hara-kiri staff."
 '())

(ly:add-interface
 'spacing-options-interface
 "Supports setting of spacing variables."
 '(spacing-increment shortest-duration-space))

(ly:add-interface
 'stanza-number-interface
 "A stanza number, to be put in from of a lyrics line."
 '())

(ly:add-interface
 'string-number-interface
 "A string number instruction."
 '())

(ly:add-interface
 'stroke-finger-interface
 "A right hand finger instruction."
 '(digit-names))

(ly:add-interface
 'system-start-text-interface
 "Text in front of the system."
 '(long-text self-alignment-X self-alignment-Y text))

(ly:add-interface
 'tab-note-head-interface
 "A note head in tablature."
 '(details))

(ly:add-interface
 'trill-spanner-interface
 "A trill spanner."
 '())

(ly:add-interface
 'trill-pitch-accidental-interface
 "An accidental for trill pitch."
 '())

(ly:add-interface
 'unbreakable-spanner-interface
 "A spanner that should not be broken across line breaks.  Override
with @code{breakable=##t}."
 '(breakable))

(ly:add-interface
 'vertically-spaceable-interface
 "Objects that should be kept at constant vertical distances.  Typically:
@rinternals{VerticalAxisGroup} objects of @rinternals{Staff} contexts."
 '())

(ly:add-interface
 'volta-interface
 "A volta repeat."
 '())
