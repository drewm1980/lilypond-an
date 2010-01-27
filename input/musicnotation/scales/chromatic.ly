
% Used for setting each semitone to different shape
#(define (shapeSemitone pitch tonic)
  (modulo (ly:pitch-semitones pitch) 12))
% Identical to standard 
#(define (shapeTonic pitch tonic)
  (let ((tonic-pitch (if (ly:pitch? tonic )
		      tonic
		      (ly:make-pitch 0 0 0))))
   (modulo (- (ly:pitch-notename pitch) (ly:pitch-notename tonic-pitch)) 7)))
% Just 3 shapes here, possibly useful for chromatic accordion tablature
#(define (shapeTritone pitch tonic)
  (modulo (ly:pitch-semitones pitch) 3))

#(define (twinline-layout pitch)
  (let* ((twintable #(0 1 0 0 0 1 0 0 0 1 0 0))
(semi (ly:pitch-semitones pitch))
)
(+ (floor (/ semi 2)) (vector-ref twintable (modulo semi 12)))))

#(define-public chromatic-data
  (list
   (cons "6-6-tetragram" (vector '(4 6 8 10) '(0 2) -1 7 12 ly:pitch-semitones #()))
   (cons "a-b"  (vector '(4 6 8 10) '(0 2) -2 7 12 ly:pitch-semitones #()))
   (cons "ailler" (vector '(4 6 8 10) '(0 2) 0 7 12 ly:pitch-semitones #()))
   (cons "5-line" (vector '(2 4 6 8 10) '(0) 0 7 12 ly:pitch-semitones #()))
   (cons "frix"  (vector '(2 4 6 8 10) '(0) -2 7 12 ly:pitch-semitones #()))
   (cons "avique" (vector '(2 3 5 7 9 10) '(0) -2 7 12 ly:pitch-semitones #()))
   (cons "mirck" (vector '(1 3 6 8 10) '() 0 7 12 ly:pitch-semitones #()))
   (cons "twinline" (vector '(2 4) '(0) 0 1 6 twinline-layout #(default do default downdo default do default downdo default do default downdo)))
   (cons "twinline-2" (vector '(4 8) '(0) 0 7 12 ly:pitch-semitones #(default do default downdo default do default downdo default do default downdo)))
   (cons "beyreuther-untitled" (vector '(4 8) '(0) 0 7 12 ly:pitch-semitones #()))
   (cons "isomorph" (vector '(0 4 6 8) '() -2 7 12 ly:pitch-semitones #()))
   (cons "kevin" (vector '(4 6 8 10) '(0 2) -1 7 12 ly:pitch-semitones #(do re downdo do re downdo do re downdo do re downdo)))
   (cons "express" (vector '(0 6) '() -2 7 12 ly:pitch-semitones #()))
   ))

#(define-public chromatic-lookup
  '( ("staff" . 0)
      ("ledger" . 1)
      ("middleCPosition" . 2)
      ("g-clef-from-c" . 3)
      ("positions-per-octave" . 4)
      ("layout" . 5)
      ("shape-note-styles" . 6)
    ))
#(define (chromatic-value notation value)
  (vector-ref (assoc-ref chromatic-data notation) (assoc-ref chromatic-lookup value)))

#(define (all-lines-notation notation first-octave last-octave)
  (all-lines first-octave last-octave
   (chromatic-value notation "staff")
   (chromatic-value notation "positions-per-octave")))

#(define (all-ledgers-notation notation first-octave last-octave)
  (all-ledgers first-octave last-octave
   (chromatic-value notation "ledger")
   (chromatic-value notation "positions-per-octave")))


#(define (all-lines first-octave last-octave single-octave positions-per-octave)
  (if (> first-octave last-octave)
   '()
   (append (map (lambda (n) (+ (* first-octave positions-per-octave) n))
	    single-octave )
    (all-lines (+ first-octave 1) last-octave single-octave positions-per-octave))))
#(define (all-ledgers first-octave last-octave single-octave positions-per-octave)
  (if (>= first-octave last-octave)
   '()
   (append (list (map (lambda (n) (+ (* (+ first-octave 1) positions-per-octave) n))
		  single-octave ))
    (all-ledgers (+ first-octave 1) last-octave single-octave positions-per-octave))))


setAltStaff = #(define-music-function (parser location style lowerOctave upperOctave)
		(string? number? number?)
		(make-music 'SequentialMusic
		 'elements
		 (list
		  (if (> (vector-length (chromatic-value style "shape-note-styles")) 0)
		   #{
		   \set shapeNoteStyles  = #(chromatic-value $style "shape-note-styles")
		   \set shapeLayoutFunction = #shapeSemitone
		   #}
(make-music 'SequentialMusic)
)
#{
		\set Staff.clefGlyph = #"clefs.G"
		\set middleCPosition = #(chromatic-value $style "middleCPosition")
		\set clefPosition = #(+ (chromatic-value $style "middleCPosition") (chromatic-value $style "g-clef-from-c"))
		\set staffLineLayoutFunction = #(chromatic-value $style "layout")
		\override Staff.StaffSymbol #'line-count = #(length (all-lines-notation $style $lowerOctave $upperOctave))
		\override Staff.StaffSymbol #'line-positions = #(all-lines-notation $style $lowerOctave $upperOctave)
		\override Staff.StaffSymbol #'internal-ledger-lines = #(all-ledgers-notation $style $lowerOctave $upperOctave)
		#}
)))


\layout {
  \context {
    \Staff
    \type "Engraver_group"
    \name "AlternativeStaff"
    \alias "Staff"
    
    \remove "Accidental_engraver"
    \remove "Key_engraver" 
    
    \description "Handles typesetting for alternative notation."
  }
  \context {
    \Score
    \accepts "AlternativeStaff"
  }
  \context {
    \StaffGroup
    \accepts "AlternativeStaff"
  }

}
