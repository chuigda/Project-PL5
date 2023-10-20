(define (make-chessboard)
    (vector 'r  'n  'b  'q  'k  'b  'n  'r
            'p  'p  'p  'p  'p  'p  'p  'p
            '() '() '() '() '() '() '() '()
            '() '() '() '() '() '() '() '()
            '() '() '() '() '() '() '() '()
            '() '() '() '() '() '() '() '()
            'P  'P  'P  'P  'P  'P  'P  'P
            'R  'N  'B  'Q  'K  'B  'N  'R))

(define chessboard-xs '(A B C D E F G H))
(define chessboard-ys '(1 2 3 4 5 6 7 8))

(define white-pieces '(R N B Q K P))
(define black-pieces '(r n b q k p))

(define (side-pieces side)
    (cond [(= side 'w) white-pieces]
          [(= side 'b) black-pieces]
          [else (error "unknown side")]))

(define (opponent-side side)
    (cond [(= side 'w) 'b]
          [(= side 'b) 'w]
          [else (error "unknown side")]))

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
          [(= char 'H) 'G]
          ; since alpha-2 is just alpha- composed with itself, if
          ; (alpha- char) is '(), then (alpha-2 char) should also be '()
          ; and that makes sense
          [(= char '()) '()]
          [else (error "unknown char index")]))

(define (alpha+ char)
    (cond [(= char 'A) 'B]
          [(= char 'B) 'C]
          [(= char 'C) 'D]
          [(= char 'D) 'E]
          [(= char 'E) 'F]
          [(= char 'F) 'G]
          [(= char 'G) 'H]
          [(= char 'H) '()]
          [(= char '()) '()] ; ditto
          [else (error "unknown char index")]))

(define (delimited+ idx)
    (cond [(= idx '()) '()]
          [(= idx 8) '()]
          [else (+ idx 1)]))

(define (delimited- idx)
    (cond [(= idx '()) '()]
          [(= idx 1) '()]
          [else (- idx 1)]))

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

(define (piece->string piece)
    (cond [(= piece 'r) "♜ "]
          [(= piece 'n) "♞ "]
          [(= piece 'b) "♝ "]
          [(= piece 'q) "♛ "]
          [(= piece 'k) "♚ "]
          [(= piece 'p) "♟ "]
          [(= piece 'R) "♖ "]
          [(= piece 'N) "♘ "]
          [(= piece 'B) "♗ "]
          [(= piece 'Q) "♕ "]
          [(= piece 'K) "♔ "]
          [(= piece 'P) "♙ "]
          [(= piece '()) "  "]
          [otherwise (error "unknown piece")]))

(define (chessboard->string chessboard)
    (define ret "")
    (define (iter-impl chessboard linear-idx)
        (if (= (% linear-idx 8) 0)
            (set! 'ret (~ ret "\n")))
        (cond [(= linear-idx 64) ret]
              [else (begin
                      (set! 'ret (~ ret
                                    (piece->string (vector-ref chessboard linear-idx))))
                      (iter-impl chessboard (+ linear-idx 1)))]))
    (iter-impl chessboard 0))
