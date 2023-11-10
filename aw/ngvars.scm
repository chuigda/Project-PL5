;; type NGVars = data nonGenericVars of Type list

(define (empty-ng-vars) (list))

(define (extend-ng-vars var ng-vars)
    (assert (= 'type-var (get-field var 'k))
            "extend-ng-vars: expected type variable")
    (assert (or (null? ng-args) (pair? ng-vars))
            "extend-ng-vars: expected type variable list")
    (cons var ng-vars))

(define (generic? type-var non-generic-vars)
    (not (occurs-in-type-list? type-var non-generic-vars)))
