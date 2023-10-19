(define (not x) (if x '() true))
(define (and x y) (if x y '()))
(define (or x y) (if x true y))

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

(define (idx->alpha idx)
    (cond [(= idx 0) 'A]
          [(= idx 1) 'B]
          [(= idx 2) 'C]
          [(= idx 3) 'D]
          [(= idx 4) 'E]
          [(= idx 5) 'F]
          [(= idx 6) 'G]
          [(= idx 7) 'H]))

(define (chessboard-ref chessboard x y)
    (define x-idx (alpha->idx x))
    (define linear-idx (+ (* 8 (- 8 y)) x-idx))
    (vector-ref chessboard linear-idx))

(define (chessboard-set! chessboard x y piece)
    (define x-idx (alpha->idx x))
    (define linear-idx (+ (* 8 (- 8 y)) x-idx))
    (vector-set! chessboard linear-idx piece))

(define (imp-move! chessboard side piece x y)
    (error "unimplemented yet"))

(define (imp-oo! chessboard side)
    (error "unimplemented yet"))

(define (imp-ooo! chessboard side)
    (error "unimplemented yet"))

(define (position-attacked? chessboard side x y)
    (error "unimplemented yet"))

(define (pawn-attacked? chessboard side x y)
    ; picking the counter side pawns
    (define pawn-piece (if (= side 'w) 'P 'p))
    ; pawns can only attack forward
    (define pawn-y (if (= side 'w) (+ y 1) (- y 1)))
    (set! 'pawn-y
        (cond [(= pawn-y 0) '()]
              [(= pawn-y 9) '()]
              [else pawn-y]))
    ; pawns can only attack diagonally
    (define pawn-x-left (if (= x 'A) '() (- (alpha->idx x) 1)))
    (define pawn-x-right (if (= x 'H) '() (+ (alpha->idx x) 1)))

    (if (= pawn-y '())
        false
        (or (and (not (= pawn-x-left '()))
                 (= (chessboard-ref chessboard pawn-x-left pawn-y) pawn-piece))
            (and (not (= pawn-x-right '()))
                 (= (chessboard-ref chessboard pawn-x-right pawn-y) pawn-piece)))))

(define chessboard-xs '(A B C D E F G H))
(define chessboard-ys '(1 2 3 4 5 6 7 8))

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

(define drill-testboard (make-chessboard))
(display (chessboard->string drill-testboard))
(display (pawn-attacked? drill-testboard 'b 'A 3))
