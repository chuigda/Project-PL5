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

(define (chessboard-ref chessboard x y)
    (define x-idx (alpha->idx x))
    (define linear-idx (+ (* 8 (- 8 y)) x-idx))
    (vector-ref chessboard linear-idx))

(define (chessboard-set! chessboard x y piece)
    (define x-idx (alpha->idx x))
    (define linear-idx (+ (* 8 (- 8 y)) x-idx))
    (vector-set! chessboard linear-idx piece))

(define (move chessboard side piece x y)
    (error "unimplemented yet"))

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
          [(= piece '()) "  "]))

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

(display (chessboard->string (make-chessboard)))
