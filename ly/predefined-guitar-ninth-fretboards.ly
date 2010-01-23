%%%% This file is part of LilyPond, the GNU music typesetter.
%%%%
%%%% Copyright (C) 2008--2010 by Jonathan Kulp
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

%  Add ninth chords to to predefined fret diagrams for standard guitar tunings

\addChordShape #'c:9 #guitar-tuning #"x;3-2;2-1;3-3-(;3-3;3-3-);"
\addChordShape #'f:9 #guitar-tuning #"1-1-(;3-3;1-1;2-2;1-1-);3-4;"

\storePredefinedDiagram \chordmode {c:9}
                        #guitar-tuning
			#(chord-shape 'c:9 guitar-tuning)
\storePredefinedDiagram \chordmode {cis:9}
                        #guitar-tuning
			#(offset-fret 1 (chord-shape 'c:9 guitar-tuning))
\storePredefinedDiagram \chordmode {des:9}
                        #guitar-tuning
			#(offset-fret 1 (chord-shape 'c:9 guitar-tuning))
\storePredefinedDiagram \chordmode {d:9}
                        #guitar-tuning
			#(offset-fret 2 (chord-shape 'c:9 guitar-tuning))
\storePredefinedDiagram \chordmode {dis:9}
                        #guitar-tuning
			#(offset-fret 3 (chord-shape 'c:9 guitar-tuning))
\storePredefinedDiagram \chordmode {ees:9}
                        #guitar-tuning
			#(offset-fret 3 (chord-shape 'c:9 guitar-tuning))
\storePredefinedDiagram \chordmode {e:9}
                        #guitar-tuning
			#"o;2-2;o;1-1;o;2-3;"
\storePredefinedDiagram \chordmode {f:9}
                        #guitar-tuning
			#(chord-shape 'f:9 guitar-tuning)
\storePredefinedDiagram \chordmode {fis:9}
                        #guitar-tuning
			#(offset-fret 1 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {ges:9}
                        #guitar-tuning
			#(offset-fret 1 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {g:9}
                        #guitar-tuning
			#(offset-fret 2 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {gis:9}
                        #guitar-tuning
			#(offset-fret 3 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {aes:9}
                        #guitar-tuning
			#(offset-fret 3 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {a:9}
                        #guitar-tuning
			#(offset-fret 4 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {ais:9}
                        #guitar-tuning
			#(offset-fret 5 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {bes:9}
                        #guitar-tuning
			#(offset-fret 5 (chord-shape 'f:9 guitar-tuning))
\storePredefinedDiagram \chordmode {b:9}
                        #guitar-tuning
			#(offset-fret -1 (chord-shape 'c:9 guitar-tuning))
