(define (make-cond cond then otherwise)
    (struct 'k         'cond
            'cond      cond
            'then      then
            'otherwise otherwise))

(define (make-lambda param body)
    (struct 'k     'lambda
            'param param
            'body  body))

(define (make-apply callee arg)
    (struct 'k      'apply
            'callee callee
            'arg    arg))

(define (make-block decl term)
    (struct 'k    'block
            'decl decl
            'term term))

(define (make-decl name term)
    (struct 'k    'decl
            'name name
            'term term))

(define (make-type-var)
    (struct 'k     'type-var
            'stamp (next-stamp!)
            'inst  '()))

(define (make-type-op op args)
    (assert (string? op)
            "make-type-op: expected string operator")
    (struct 'k    'type-op
            'op   op
            'args args))

(define (same-var? var1 var2)
    (= (get-field var1 'stamp)
       (get-field var2 'stamp)))

(define (prune! type)
    (define k (get-field type 'k))
    (cond [(= k 'type-var)
           (begin (define inst (get-field type 'inst))
                  (if (null? inst)
                      type
                      (begin (define pruned (prune! inst))
                             (set-field! type 'inst pruned)
                             pruned)))]
          [(= k 'type-op) type]
          [otherwise (error "prune!: unknown `type` discriminant: " k)]))

(define (occurs-in-type? type-var type)
    (assert (= 'type-var (get-field type-var 'k))
            "occurs-in-type?: expected type variable")
    (set! 'type (prune! type))
    (define k (get-field type 'k))
    (cond [(= k 'type-var) (same-var? type-var type)]
          [(= k 'type-op) (occurs-in-type-list? type-var (get-field type 'args))]
          [otherwise (error "occurs-in-type?: unknown `type` discriminant: " k)]))

(define (occurs-in-type-list? type-var type-list)
    (exists? (lambda (t) (occurs-in-type? type-var t))
             type-list))

(define bool-type (make-type-op "bool" null))

(define int-type (make-type-op "int" null))

(define (make-func-type from into) (new-type-op "func" (list from into)))
