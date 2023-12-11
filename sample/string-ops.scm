(define (play-with-strings a b)
    (~ (~ a a) ", " (~ b b) ", " (~ a b)))

(display (play-with-strings "Hello" "World"))

(define s "AbstractProxyFactoryBean")
(display (string-ref s 0))
(string-set! s 0 66)
(display s)
