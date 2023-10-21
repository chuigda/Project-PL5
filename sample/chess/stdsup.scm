; supplements to MiniCement stdlib

; these functions does not implement short-circuit evaluation, use with care!
(define (and x y) (if x y false))
(define (or x y) (if x true y))
(define (id x) x)

(define (not x) (if x false true))

(define (or-list xs)
    (if (= xs '())
        false
        (or (car xs) (or-list (cdr xs)))))

(define (contains? list elem)
    (cond [(= list '()) false]
          [(= (car list) elem) true]
          [else (contains? (cdr list) elem)]))

(define (abs x)
    (if (< x 0)
        (- 0 x)
        x))
