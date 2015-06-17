(define (compose f g) (lambda (x) (f (g x))))
(define cadr (compose car cdr))
(define caar (compose car car))
(define cddr (compose cdr cdr))
(define cdar (compose cdr car))

(define (append x y)
    (if (null? x)
        y
        (cons (car x) (append (cdr x) y))))

(define (map f ls)
 (if (null? ls)
  '()
  (cons (f (car ls))
        (map f (cdr ls)))))

(defmacro (let bindings . stms) 
 (append (list (list 'lambda (map car bindings) (cons 'begin stms)))
     (map cadr bindings)))

(define (cond->if pe-list)
 (if (null? pe-list)
  '()
  (let ((pair (car pe-list))
        (rest (cdr pe-list)))
   (let ((p1 (car pair))
         (e1 (cadr pair)))
    (if (eq? p1 'else)
     e1
     `(if ,p1 ,e1 ,(cond->if rest)))))))

(defmacro (cond . pairs)
    (cond->if pairs))

(define (range s e)
 (if (= s e)
  '()
  (cons s (range (+ s 1) e))))

(define (newline) (print "\n"))
(print (apply + (range 0 500)))
(newline)

(let ((x 1)) (print (+ x 3)))

(cond (#t (print 1)) (#f (print 2)))
(cond (#f (print 1)) (else (print 2)))

