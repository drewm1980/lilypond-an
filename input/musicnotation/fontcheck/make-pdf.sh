#! /bin/sh

lilypond -o twinline -e '(define notation-style "'"twinline"'")' fontcheck

rm -f *.{eps,tex,texi,ps,png}

open twinline.pdf
