(define (make-chessboard)
    (vector 'r  'n  'b  'q  'k  'b  'n  'r
            'p  'p  'p  'p  'p  'p  'p  'p
            '() '() '() '() '() '() '() '()
            '() '() '() '() '() '() '() '()
            '() '() '() '() '() '() '() '()
            '() '() '() '() '() '() '() '()
            'P  'P  'P  'P  'P  'P  'P  'P
            'R  'N  'B  'Q  'K  'B  'N  'R))

(define (alpha->idx char)
    (cond [(= char 'A) 0]
          [(= char 'B) 1]
          [(= char 'C) 2]
          [(= char 'D) 3]
          [(= char 'E) 4]
          [(= char 'F) 5]
          [(= char 'G) 6]
          [(= char 'H) 7]))

(define (alpha- char)
    (cond [(= char 'A) '()]
          [(= char 'B) 'A]
          [(= char 'C) 'B]
          [(= char 'D) 'C]
          [(= char 'E) 'D]
          [(= char 'F) 'E]
          [(= char 'G) 'F]
          [(= char 'H) 'G]))

(define (alpha+ char)
    (cond [(= char 'A) 'B]
          [(= char 'B) 'C]
          [(= char 'C) 'D]
          [(= char 'D) 'E]
          [(= char 'E) 'F]
          [(= char 'F) 'G]
          [(= char 'G) 'H]
          [(= char 'H) '()]))

(define (delimited+ idx) (if (= idx 8) '() (+ idx 1)))
(define (delimited- idx) (if (= idx 1) '() (- idx 1)))

(define (alpha+2 x) (alpha+ (alpha+ x)))
(define (alpha-2 x) (alpha- (alpha- x)))
(define (delimited+2 x) (delimited+ (delimited+ x)))
(define (delimited-2 x) (delimited- (delimited- x)))

(define (chessboard-ref chessboard x y)
    (define x-idx (alpha->idx x))
    (define linear-idx (+ (* 8 (- 8 y)) x-idx))
    (vector-ref chessboard linear-idx))

(define (chessboard-set! chessboard x y piece)
    (define x-idx (alpha->idx x))
    (define linear-idx (+ (* 8 (- 8 y)) x-idx))
    (vector-set! chessboard linear-idx piece))
