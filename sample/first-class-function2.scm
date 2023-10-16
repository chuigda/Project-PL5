(define (my-cons x y)
    (lambda (op arg)
        (cond [(= op 'car) x]
              [(= op 'cdr) y]
              [(= op 'set-car!) (set! x arg)]
              [(= op 'set-cdr!) (set! y arg)])))

(define (my-car z) (z 'car '()))
(define (my-cdr z) (z 'cdr '()))
(define (my-set-car! z new-car) ((z 'set-car!) new-car))
(define (my-set-cdr! z new-cdr) ((z 'set-cdr!) new-cdr))

(define a (my-cons 1 2))
(define b (my-cons "Zdravstvuyte" 'mir))

(display (my-car a) (my-cdr a))
(display (my-car b) "," (my-cdr b))
