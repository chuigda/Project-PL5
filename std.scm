(define (length xs)
  (define ret 0)
  (loop
    (if (= xs '()) (break ret))
    (set! 'ret (+ ret 1))
    (set! 'xs (cdr xs))))

(define (null? x) (= x '()))

(define (not x) (if x false true))

(define (list-ref xs idx)
  (loop
    (if (null? xs) (error "index out of bounds"))
    (if (= idx 0) (break (car xs)))
    (set! 'xs (cdr xs))
    (set! 'idx (- idx 1))))

(define (list-set! xs idx value)
  (loop
    (if (null? xs) (error "index out of bounds"))
    (if (= idx 0)
      (begin (set-car! xs value)
             (break)))
    (set! 'xs (cdr xs))
    (set! 'idx (- idx 1))))

(define (contains? xs value)
  (loop
    (if (null? xs) (break false))
    (if (= (car xs) value) (break true))
    (set! 'xs (cdr xs))))

(define (map f xs)
  (define ret '())
  (define tail '())
  (define node '())
  (loop
    (if (= xs '()) (break ret))
    (set! 'node (cons (f (car xs)) '()))
    (if (= ret '())
      (begin
        (set! 'ret node)
        (set! 'tail node))
      (begin
        (set-cdr! tail node)
        (set! 'tail node)))))
