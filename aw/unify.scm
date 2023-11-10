(define (unity-type! type1 type2)
    (set! 'type1 (prune! type1))
    (set! 'type2 (prune! type2))

    (define k1 (get-field type1 'k))
    (define k2 (get-field type2 'k))
    (cond [(= k1 'type-var)
           (if (occurs-in-type? type1 type2)
               (cond [(= k2 'type-var) '()]
                     [(= k2 'type-op) (error "unify-type!: unification error: " k1 " vs " k2)]
                     [otherwise (error "unify-type!: unknown type discriminant: " k2)])
               (begin (set-field! type1 'inst type2)
                      '()))]
          [(= k1 'type-op)
           (cond [(= k2 'type-var) (unify-type type2 type1)]
                 [(= k2 'type-op)
                  (begin (define op1 (get-field type1 'op))
                         (define op2 (get-field type2 'op))
                         (if (= op1 op2)
                             (unify-args! (get-field type1 'args)
                                          (get-field type2 'args))
                             (error "unify-type!: unification error: different operator " op1 " vs " op2)))]
                 [otherwise (error "unify-type!: unkown type discriminant: " k2)])]
          [otherwise (error "unify-type!: unkown type discriminant: " k1)]))

(define (unify-args! args1 args2)
    (cond [(and (null? args1) (null? args2)) '()]
          [(and (not (null? args1)) (not (null? args2)))
           (begin (unify-type! (car args1) (car args2))
                  (unify-args! (cdr args1) (cdr args2)))]
          [otherwise (error "unify-args!: unification error: different number of arguments")]))
