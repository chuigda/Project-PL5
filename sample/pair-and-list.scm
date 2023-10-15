(define ls (list 1 2 3 '1 '2 '3))
(display ls)

(define (display-one-by-one ls)
    (cond [(= ls '()) (begin)]
          [else (begin
                    (display (car ls))
                    (display-one-by-one (cdr ls)))]))
(display-one-by-one ls)
