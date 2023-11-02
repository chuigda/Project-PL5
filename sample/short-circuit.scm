(define a (and false (error "will not be executed")))
(define b (or true (error "will not be executed")))

(println "a = " a ", b = " b)

(define (call-fn-1 fn)
    (fn false (error "will not be executed")))

(define (call-fn-2 fn)
    (fn true (error "will not be executed")))

(println "(call-fn-1 and) = " (call-fn-1 and))
(println "(call-fn-2 or) = " (call-fn-2 or))
