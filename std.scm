(define (<= x y) (or (< x y) (= x y)))
(define (> x y) (< y x))
(define (>= x y) (or (> x y) (= x y)))
(define (!= x y) (not (= x y)))

(define (caar x) (car (car x)))
(define (cadr x) (car (cdr x)))
(define (cdar x) (cdr (car x)))
(define (cddr x) (cdr (cdr x)))
(define (caaar x) (car (car (car x))))
(define (caadr x) (car (car (cdr x))))
(define (cadar x) (car (cdr (car x))))
(define (caddr x) (car (cdr (cdr x))))
(define (cdaar x) (cdr (car (car x))))
(define (cdadr x) (cdr (car (cdr x))))
(define (cddar x) (cdr (cdr (car x))))
(define (cdddr x) (cdr (cdr (cdr x))))
(define (cadddr x) (car (cdr (cdr (cdr x)))))

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
        (set! 'tail node)))
    (set! 'xs (cdr xs))))

(define (foreach f xs)
  (loop
    (if (= xs '()) (break))
    (f (car xs))
    (set! 'xs (cdr xs))))

(define (foldi f init xs)
  (loop
    (if (null? xs) (break init))
    (set! 'init (f init (car xs)))
    (set! 'xs (cdr xs))))

(define (assert x err) (if (not x) (error err)))
