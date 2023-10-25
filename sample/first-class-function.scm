(define (fst x y) x)
(define (snd x y) y)

(define (my-cons x y)
  (lambda (m) (m x y)))

(define (my-car z) (z fst))
(define (my-cdr z) (z snd))

(define a (my-cons 1 2))
(define b (my-cons "Zdravstvuyte" 'mir))

(display (my-car a) (my-cdr a))
(display (my-car b) "," (my-cdr b))

(print (my-car a) "," (my-cdr a))
