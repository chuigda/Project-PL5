(define list1 (list 1 2 3 '1 '2 '3))
(define list2 '(1 2 3 a b c))

(define (display-one-by-one ls)
    (cond [(= ls '()) (begin)]
          [else (begin
                    (display (car ls))
                    (display-one-by-one (cdr ls)))]))

(display list1)
(display-one-by-one list1)

(display list2)
(display-one-by-one list2)

(set-car! list1 114)
(set-car! (cdr list1) 514)
(set-car! (cdr (cdr list1)) 1919)
(set-car! (cdr (cdr (cdr list1))) 810)
(display list1)
