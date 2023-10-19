(define (and x y) (if x y false))
(define (or x y) (if x true y))
(define (id x) x)

(define (or-list xs)
    (if (= xs '())
        false
        (or (car xs) (or-list (cdr xs)))))