(define g-stamp-counter 0)

(define (next-stamp!)
    (set! 'g-stamp-counter (+ g-stamp-counter 1))
    g-stamp-counter)
