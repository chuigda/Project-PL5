;; type CopyEnv = (Type * Type) list

(define (fresh-type type ng-vars)
    (define (fresh! type env)
        (set! 'type (prune! type)))
        (define k (get-field type 'k))
        (cond [(= k 'type-var)
               (if (generic? type ng-vars)
                   (fresh-var! type (ref-value env) env)
                   type)]
              [(= k 'type-op)
               (make-type-op (get-field type 'op)
                             (map (lambda (arg) (fresh! arg env))
                                  (get-field type 'args)))])
    (define (fresh-var! var scan env)
        (if (null? scan)
            (begin (define new-var (make-type-var))
                   (ref-set-value! env
                                   (cons (cons var new-var)
                                         (ref-value env))))
            (begin (define curr (car scan))
                   (define old-var (car curr))
                   (define new-var (cdr curr))
                   (if (same-var? var old-var)
                       new-var
                       (fresh-var! var (cdr scan) env)))))
    (fresh! type (ref-value (list))))
