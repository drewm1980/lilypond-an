#! /bin/sh
source ../notations.sh
for notation in ${NOTATIONS}; 
do lilypond -o bach-invention-09-${notation} -e '(define notation-style "'"${notation}"'")' bach-invention-09-template;
done
lilypond bach-invention-09-traditional
rm *.ps
