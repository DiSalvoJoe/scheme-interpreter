(define (map f ls)
 (if (null? ls)
  '()
  (cons (f (car ls))
        (map f (cdr ls)))))

(define (range s e)
 (if (= s e)
  '()
  (cons s (range (+ s 1) e))))

(define (newline) (print "\n"))

(print (apply + (range 0 500)))
(newline)
