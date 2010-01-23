;;;; This file is part of LilyPond, the GNU music typesetter.
;;;;
;;;; Copyright (C) 2004--2010 Han-Wen Nienhuys <hanwen@xs4all.nl>
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

;; TODO:
;;
;; lookup-font should be written in  C.
;;

(define-class <Font-tree-element>
  ())

(define-class <Font-tree-leaf> (<Font-tree-element>)
  (default-size #:init-keyword #:default-size)
  (size-vector  #:init-keyword #:size-vector))

(define-class <Font-tree-node> (<Font-tree-element>)
  (qualifier #:init-keyword #:qualifier  #:accessor font-qualifier)
  (default #:init-keyword #:default #:accessor font-default)
  (children #:init-keyword #:children #:accessor font-children))

(define (make-font-tree-leaf size size-font-vector)
  (make <Font-tree-leaf> #:default-size size #:size-vector size-font-vector))

(define (make-font-tree-node
	 qualifier default)
  (make <Font-tree-node>
    #:qualifier qualifier
    #:default default
    #:children (make-hash-table 11)))

(define-method (display (leaf <Font-tree-leaf>) port)
  (map (lambda (x) (display x port))
       (list
	"#<Font-size-family:\n"
	(slot-ref leaf 'default-size)
	(slot-ref leaf 'size-vector)
	"#>"
	)))

(define-method (display (node <Font-tree-node>) port)
  (map
   (lambda (x)
     (display x port))
   (list
    "Font_node {\nqual: "
    (font-qualifier node)
    "(def: "
    (font-default node)
    ") {\n"))
  (for-each
   (lambda (x)
     (display "\n")
     (display (car x) port)
     (display "=" port)
     (display (cdr x) port))
   (hash-table->alist (font-children node)))
  (display "} }\n"))

(define default-qualifier-order
  '(font-encoding font-family font-shape font-series))

(define-method (add-font (node <Font-tree-node>) fprops size-family)
  (define (assoc-delete key alist)
    (assoc-remove! (list-copy alist) key))

  (define (make-node fprops size-family)
    (if (null? fprops)
	(make-font-tree-leaf (car size-family) (cdr size-family))
	(let* ((qual (next-qualifier default-qualifier-order fprops)))
	  (make-font-tree-node qual
			       (assoc-get qual fprops)))))

  (define (next-qualifier order props)
    (cond
     ((and (null? props) (null? order))
      #f)
     ((null? props) (car order))
     ((null? order) (caar props))
     (else
      (if (assoc-get (car order) props)
	  (car order)
	  (next-qualifier (cdr order) props)))))

  (let* ((q (font-qualifier node))
	 (d (font-default node))
	 (v (assoc-get q fprops d))
	 (new-fprops (assoc-delete q fprops))
	 (child (hashq-ref (slot-ref node 'children)
			   v #f)))
    (if (not child)
	(begin
	  (set! child (make-node new-fprops size-family))
	  (hashq-set! (slot-ref node 'children) v child)))
    (if (pair? new-fprops)
	(add-font child new-fprops size-family))))

(define-method (add-font (node <Font-tree-leaf>) fprops size-family)
  (throw "must add to node, not leaf"))

(define-method (g-lookup-font (node <Font-tree-node>) alist-chain)
  (let* ((qual (font-qualifier node))
	 (def (font-default node))
	 (val (chain-assoc-get qual alist-chain def))
	 (desired-child (hashq-ref (font-children node) val)))

    (if desired-child
	(g-lookup-font desired-child alist-chain)
	(g-lookup-font (hashq-ref (font-children node) def) alist-chain))))

(define-method (g-lookup-font (node <Font-tree-leaf>) alist-chain)
  node)

;; two step call is handy for debugging.
(define (lookup-font node alist-chain)
  (g-lookup-font node alist-chain))


;; Ugh.  Currently, we load the PFB Feta fonts for `fetaDynamic' with
;; Pango.  This should be changed to load the Emmentaler fonts instead
;; (with Pango too), but then we need support for a `font-style'
;; property which isn't implemented yet.
(define feta-alphabet-size-vector
  (list->vector
   (map (lambda (tup)
	  (cons (ly:pt (cdr tup))
		(format "feta-alphabet~a ~a"
			(car tup)
			(ly:pt (cdr tup)))))
	'((11 . 11.22)
	  (13 . 12.60)
	  (14 .  14.14)
	  (16 . 15.87)
	  (18 . 17.82)
	  (20 . 20)
	  (23 . 22.45)
	  (26 . 25.20)))))

;; Each size family is a vector of fonts, loaded with a delay.  The
;; vector should be sorted according to ascending design size.
(define-public (add-music-fonts node name family factor)
  (for-each
   (lambda (x)
     (add-font node
	       (list (cons 'font-encoding (car x))
		     (cons 'font-family family))
	       (cons (* factor (cadr x))
		     (caddr x))))
   `((fetaDynamic ,(ly:pt 20.0) ,feta-alphabet-size-vector)
     (fetaNumber ,(ly:pt 20.0) ,feta-alphabet-size-vector)
     (fetaMusic ,(ly:pt 20.0)
		#(
		  ,(delay (ly:system-font-load (string-append name "-11")))
		  ,(delay (ly:system-font-load (string-append name "-13")))
		  ,(delay (ly:system-font-load (string-append name "-14")))
		  ,(delay (ly:system-font-load (string-append name "-16")))
		  ,(delay (ly:system-font-load (string-append name "-18")))
		  ,(delay (ly:system-font-load (string-append name "-20")))
		  ,(delay (ly:system-font-load (string-append name "-23")))
		  ,(delay (ly:system-font-load (string-append name "-26")))
		  ))
     (fetaBraces ,(ly:pt 20.0)
		 #(,(delay (ly:system-font-load
			    ;;; TODO: rename aybabtu to emmentaler-brace
			    (if (string=? name "emmentaler")
				"aybabtu"
				(string-append name "-brace"))
			    )))))))

(define-public (add-pango-fonts node lily-family family factor)
  (define (add-node shape series)
    (add-font node
	      `((font-family . ,lily-family)
		(font-shape . ,shape)
		(font-series . ,series)
		(font-encoding . latin1) ;; ugh.
		)
	      `(,(ly:pt (* factor 11.0))
		. #(,(cons
		     (ly:pt 12)
		     (ly:make-pango-description-string
		       `(((font-family . ,family)
			  (font-series . ,series)
			  (font-shape . ,shape)))
		       (ly:pt 12)))))))

  (add-node 'upright 'normal)
  (add-node 'caps 'normal)
  (add-node 'upright 'bold)
  (add-node 'italic 'normal)
  (add-node 'italic 'bold))

(define-public (make-pango-font-tree roman-str sans-str typewrite-str factor)
  (let ((n (make-font-tree-node 'font-encoding 'fetaMusic)))
    (add-music-fonts n "emmentaler" 'feta factor)
;; Let's not do this [yet], see input/regression/gonville.ly
;;    (add-music-fonts n "gonville" 'gonville factor)
    (add-pango-fonts n 'roman roman-str factor)
    (add-pango-fonts n 'sans sans-str factor)
    (add-pango-fonts n 'typewriter typewrite-str factor)
    n))

(define-public (make-century-schoolbook-tree factor)
  (make-pango-font-tree
    "Century Schoolbook L,serif"
    "sans-serif" "monospace" factor))

(define-public all-text-font-encodings
  '(latin1))

(define-public all-music-font-encodings
  '(fetaBraces
    fetaDynamic
    fetaMusic
    fetaNumber))

(define-public (magstep s)
  (exp (* (/ s 6) (log 2))))

(define-public (magnification->font-size m)
  (* 6 (/ (log m) (log 2))))
