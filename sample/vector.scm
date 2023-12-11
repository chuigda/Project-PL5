(define v (vector 1 2 3 4 5))
(display v "of length" (vector-length v))

(display "(vector-ref v 0)" (vector-ref v 0))
(display "(vector-ref v 1)" (vector-ref v 1))
(vector-set! v 2 114)
(vector-set! v 3 514)

(display "(vector-ref v 2)" (vector-ref v 2))
(display "(vector-ref v 3)" (vector-ref v 3))

(vector-push! v 'sym)
(vector-push! v "string")
(display v "of length" (vector-length v))
(display "(vector-ref v 5)" (vector-ref v 5))
(display "(vector-ref v 6)" (vector-ref v 6))
