(define (analyse-term! term type-env ng-vars)
    (define k (get-field term 'k))
    (cond [(= k )]