(define (a) (b))

(define (b) (c))

(define (c) (d))

(define (d) (error "fatal error occurred!"))

(a)
