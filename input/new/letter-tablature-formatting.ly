\version "2.11.61"

\header {
  lsrtags = "staff-notation,fretted-strings"

  texidoc = "Tablature can be formatted using letters instead of
numbers."
  doctitle = "Letter tablature formatting"
}

#(define (letter-tablature-format str context event)
  (let*
      ((tuning (ly:context-property context 'stringTunings))
       (pitch (ly:event-property event 'pitch)))
    (make-whiteout-markup
     (make-vcenter-markup
      (string (integer->char
         (+ (char->integer #\a)
            (- (ly:pitch-semitones pitch)
            (list-ref tuning (- str 1))))))))))

music = \relative c {
  c4 d e f
  g4 a b c
  d4 e f g
}

<<
  \new Staff {
    \clef "G_8"
    \music
  }
  \new TabStaff \with { 
    tablatureFormat = #letter-tablature-format
  }
  {
    \music
  }
>>
