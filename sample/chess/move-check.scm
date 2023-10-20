(define (can-move? chessboard side start-x start-y end-x end-y)
    (define piece (chessboard-ref chessboard start-x start-y))
    (define dest-piece (chessboard-ref chessboard end-x end-y))
    (cond [(and (= start-x x) (= start-y y)) false]
          [(= piece '()) false]
          [(not (contains? (side-pieces side) piece)) false]
          [(contains (side-pieces side) dest-piece) false]
          [(not (valid-move? chessboard start-x start-y end-x end-y piece)) false]))

(define (valid-move? chessboard side start-x start-y end-x end-y piece)
    (cond [(or (= piece 'R) (= piece 'r))
           (check-move-hv? chessboard side start-x start-y end-x end-y)]
          [(or (= piece 'B) (= piece 'b))
           (check-move-diag? chessboard side start-x start-y end-x end-y)]
          [(or (= piece 'Q) (= piece 'q))
           (or (check-move-hv? chessboard side start-x start-y end-x end-y)
               (check-move-diag? chessboard side start-x start-y end-x end-y))]
          [(or (= piece 'K) (= piece 'k))
           (check-move-king? chessboard side start-x start-y end-x end-y)]
          [(or (= piece 'N) (= piece 'n))
            (check-move-knight? chessboard side start-x start-y end-x end-y)]
          [(or (= piece 'P) (= piece 'p))
            (check-move-pawn? chessboard side start-x start-y end-x end-y)]
          [else (error "invalid piece")]))

; check if a piece of side can move horizontally or vertically from
; (start-x, start-y) to (end-x, end-y), meaning that: no piece is in the way,
; and the destination is not occupied by a piece of the same side
(define (check-move-hv? chessboard side start-x start-y end-x end-y)
    (cond [(= start-x end-x) (check-move-vert? chessboard side start-y end-y start-x)]
          [(= start-y end-y) (check-move-horiz? chessboard side start-x end-x start-y)]
          [else false]))

(define (check-move-horiz? chessboard side start-x end-x y)
    (if (< start-x end-x)
        (imp-check-move-horiz? chessboard side start-x end-x y alpha+)
        (imp-check-move-horiz? chessboard side start-x end-x y alpha-)))

(define (imp-check-move-horiz? chessboard side start-x end-x y alpha)
    (cond [(= start-x end-x)
           (not (contains? (side-pieces side)
                           (chessboard-ref chessboard alpha start-x y)))]
          [(not (= '() (chessboard-ref chessboard alpha start-x y))) false]
          [else (imp-check-move-horiz? chessboard side alpha end-x y alpha)]))

(define (check-move-vert? chessboard side start-y end-y x)
    (if (< start-y end-y)
        (imp-check-move-vert? chessboard side start-y end-y x +)
        (imp-check-move-vert? chessboard side start-y end-y x -)))

(define (imp-check-move-vert? chessboard side start-y end-y x delta)
    (cond [(= start-y end-y)
           (not (contains? (side-pieces side)
                           (chessboard-ref chessboard x start-y delta)))]
          [(not (= '() (chessboard-ref chessboard x start-y delta))) false]
          [else (imp-check-move-vert? chessboard side delta end-y x delta)]))

; check if a piece of side can move diagonally from (start-x, start-y) to
; (end-x, end-y), meaning that: (abs (start-x - end-x)) should be equal to
; (abs (start-y - end-y)), and no piece is in the way, and the destination is
; not occupied by a piece of the same side
(define (check-move-diag? chessboard side start-x start-y end-x end-y)
    (define dx (- end-x start-x))
    (define dy (- end-y start-y))
    (define dx-fn (if (< dx 0) alpha- alpha+))
    (define dy-fn (if (< dy 0) - +))
    (if (not (= (abs dx) (abs dy)))
        false
        (imp-check-move-diag? chessboard side start-x start-y end-x end-y dx-fn dy-fn)))

(define (imp-check-move-diag? chessboard side start-x start-y end-x end-y dx-fn dy-fn)
    (cond [(and (= start-x end-x) (= start-y end-y))
           (not (contains? (side-pieces side)
                           (chessboard-ref chessboard start-x start-y)))]
          [(not (= '() (chessboard-ref chessboard start-x start-y))) false]
          [else (imp-check-move-diag? chessboard
                                      side
                                      (dx-fn start-x)
                                      (dy-fn start-y)
                                      end-x
                                      end-y
                                      dx-fn
                                      dy-fn)]))

(define (check-move-king? chessboard side start-x start-y end-x end-y)
    ; ensure that moving just 1 square in any direction
    ; and target square is not occupied by a piece of the same side
    (define dx (abs (- end-x start-x)))
    (define dy (abs (- end-y start-y)))
    (and (or (= dx 1) (= dy 1))
         (not (contains? (side-pieces side)
                         (chessboard-ref chessboard end-x end-y)))))

(define (check-move-knight? chessboard side start-x start-y end-x end-y)
    ; ensure that moving 2 squares in one direction and 1 square in the other
    ; and target square is not occupied by a piece of the same side
    (define dx (abs (- end-x start-x)))
    (define dy (abs (- end-y start-y)))
    (and (or (and (= dx 2) (= dy 1))
             (and (= dx 1) (= dy 2)))
         (not (contains? (side-pieces side)
                         (chessboard-ref chessboard end-x end-y)))))

(define (check-move-pawn? chessboard side start-x start-y end-x end-y)
    (define dx (- end-x start-x))
    (define dy (- end-y start-y))
    (define abs-dx (abs dx))
    (define abs-dy (abs dy))
    ; pawns can only move forward, meaning that if side is white, dy should be
    ; positive, and if side is black, dy should be negative
    (cond [(and (= side 'w') (< dy 0)) false]
          [(and (= side 'b') (> dy 0)) false]
          [(and (= abs-dx 1) (= abs-dy 1))
           ; pawns can move diagonally if that would capture an opponent's piece
           ; or if that would be an en passant move (not implemented yet)
           (contains? (side-pieces (opposite side))
                      (chessboard-ref chessboard end-x end-y))]
          [(= abs-dx 0)
           ; pawns can move forward, 2 squares if it's the first move, or 1
           ; square otherwise, but only if the target square is not occupied
           (begin
               (define max-moves
                       (cond [(and (= side 'w') (= start-y 2)) 2]
                             [(and (= side 'b') (= start-y 7)) 2]
                             [else 1]))
               (if (> abs-dy max-moves)
                   false
                   (= '() (chessboard-ref chessboard end-x end-y))))))
