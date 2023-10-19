(define (and x y) (if x y false))
(define (or x y) (if x true y))

(define (id x) x)

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

(define (has-attacker-at-position-list? chessboard x y positions attacker)
    (if (= positions '())
        false
        (if (has-attacker-at-position? chessboard x y (car positions) attacker)
            true
            (has-attacker-at-position-list? chessboard x y (cdr positions) attacker))))

(define (has-attacker-at-position? chessboard x y position attacker)
    (if (or (= (car position) '())
               (= (cdr position) '()))
        false
        (= (chessboard-ref chessboard (car position) (cdr position)) attacker))) 

(define (pawn-attack? chessboard side x y)
    (define pawn-piece (if (= side 'w) 'P 'p))
    (define pawn-y (if (= side 'w) (delimited- y) (delimited+ y)))
    (define pawn-x-left (alpha- x))
    (define pawn-x-right (alpha+ x))
    (has-attacker-at-position-list? chessboard
                                    x
                                    y
                                    (list (cons pawn-x-left pawn-y)
                                          (cons pawn-x-right pawn-y))
                                    pawn-piece))

(define (has-attacker? chessboard x y dx dy attacker)
    (cond [(= x '()) false]
          [(= y '()) false]
          [else (begin
              (define this-piece (chessboard-ref chessboard x y))
              (cond [(= this-piece attacker) true]
                    [(= this-piece '())
                     (has-attacker? chessboard (dx x) (dy y) dx dy attacker)]
                    [else false]))]))

(define (rook-attack? chessboard side x y)
    (define rook-piece (if (= side 'w) 'R 'r))
    (or (or (has-attacker? chessboard x y alpha- id rook-piece)
            (has-attacker? chessboard x y alpha+ id rook-piece))
        (or (has-attacker? chessboard x y id delimited- rook-piece)
            (has-attacker? chessboard x y id delimited+ rook-piece))))

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
(chessboard-set! drill-testboard 'A 2 '())
(display (chessboard->string drill-testboard))
(display "Is there a white pawn attacking A2?"
         (pawn-attack? drill-testboard 'w 'A 2))
(display "Is there a white pawn attacking A3?"
         (pawn-attack? drill-testboard 'w 'A 3))
(display "Is there a white pawn attacking A4?"
         (pawn-attack? drill-testboard 'w 'A 4))
(display "Is there a white rook attacking A3?"
         (rook-attack? drill-testboard 'w 'A 3))
(display "Is there a black rook attacking H3?"
         (rook-attack? drill-testboard 'b 'H 3))