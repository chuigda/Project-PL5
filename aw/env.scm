; type TypeEnv = data typeEnv of (Ide * Type) list;

(define (empty-type-env) (list))

(define (extend-type-env bind type type-env)
    (cons (cons bind type) type-env))

(define (retrieve-type-env ident type-env ng-vars)
    (define cur-node '())
    (loop
        (if (null? type-env)
            (error "retrieve-type-env: undefined identifier: " ident))
        (set! 'cur-node (car type-env))
        (if (= (car cur-node) ident)
            (fresh-type (cdr cur-node) ng-vars))
        (set! 'type-env (cdr type-env))))
