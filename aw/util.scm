(define (foldr f list init)
    (if (null? list) init
        (f (car list) (foldr f (cdr list) init))))

(define (exists? pred list)
    (loop
        (if (null? list) (break false))
        (if (pred (car list)) (break true))
        (set! 'list (cdr list))))

(define (ref value)
        (struct 'k 'ref 'value value))

(define (ref-value r)
    (assert (= (get-field r 'k) 'ref)
            "ref-value: expected reference")
    (get-field r 'value))

(define (ref-set-value! r value)
    (assert (= (get-field r 'k) 'ref)
            "ref-set-value!: expected reference")
    (set-field! r 'value value))
