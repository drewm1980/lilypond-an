#! /bin/sh
for notation in 6-6-tetragram avique mirck twinline twinline-2 kevin 5-line frix a-b ailler express beyreuther-untitled isomorph; 
do lilypond -o bach-invention-09-${notation} -e '(define notation-style "'"${notation}"'")' bach-invention-09-template;
done
lilypond bach-invention-09-traditional
rm *.ps
