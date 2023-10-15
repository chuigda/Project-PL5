(define (play-with-strings a b)
    (~ (~ a a) ", " (~ b b) ", " (~ a b)))

(display (play-with-strings "Hello" "World"))
