(define v (vector 1 2 3 4 5))
(display v "of length" (vector-length v))

(display "(vector-at v 0)" (vector-at v 0))
(display "(vector-at v 1)" (vector-at v 1))
(vector-set! v 2 114)
(vector-set! v 3 514)

(display "(vector-at v 2)" (vector-at v 2))
(display "(vector-at v 3)" (vector-at v 3))

(vector-push! v 'sym)
(vector-push! v "string")
(display v "of length" (vector-length v))
(display "(vector-at v 5)" (vector-at v 5))
(display "(vector-at v 6)" (vector-at v 6))
