#! /bin/sh
source ../notations.sh
for notation in ${NOTATIONS}; do
    lilypond -o chords-${notation} -e '(define notation-style "'"${notation}"'")' chords-template
done
notation=traditional
lilypond chords-traditional
rm -f *.{eps,tex,texi,ps}
