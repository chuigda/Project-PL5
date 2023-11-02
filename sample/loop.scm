(define (sum-imperative xs)
    (define total 0)
    (loop
        (if (= xs '())
            (break total))
        (set! 'total (+ total (car xs)))
        (set! 'xs (cdr xs))))

(define xs (list 1 2 3 4 5))

(print (sum-imperative xs) "\n")
