(define (imp-move! chessboard side start-x start-y end-x end-y)
    ; first, the piece must could move from start to end
    (if (not (can-move? chessboard side start-x start-y end-x end-y))
        (begin
            (display "Invalid move from" start-x start-y "to" end-x end-y)
            '())
        ; after the move, king must not be checked
        (begin
            (define king-piece (if (= side 'w) 'K 'k))
            ; if we are moving the king itself, we need to ensure that
            ; the king will not be checked after the move
            (if (= king-piece (chessboard-ref chessboard start-x start-y))
                (if (position-attacked? chessboard
                                        (opponent-side side)
                                        end-x
                                        end-y)
                    (begin (display "Cannot move from" start-x start-y "to" end-x end-y ", king will be checked")
                           '())
                    ; if all the above conditions are satisfied, then move the piece
                    (begin
                        (chessboard-set! chessboard start-x start-y '())
                        (chessboard-set! chessboard end-x end-y king-piece)
                        true))
                (begin
                    (define king-xy (find-piece chessboard king-piece))
                    (if (position-attacked? chessboard
                                            (opponent-side side)
                                            (car king-xy)
                                            (cdr king-xy))
                        (begin
                            (display "Cannot move from" start-x start-y "to" end-x end-y ", king will be checked")
                            '())
                        ; if all the above conditions are satisfied, then move the piece
                        (begin
                            (define piece (chessboard-ref chessboard start-x start-y))
                            (chessboard-set! chessboard start-x start-y '())
                            (chessboard-set! chessboard end-x end-y piece)
                            true)))))))

(define (imp-oo! chessboard side)
    ; the rook and king should not have been moved, that should have been
    ; checked before calling this function
    (define rook-xy (if (= side 'w) (cons 'H 1) (cons 'H 8)))
    (define king-xy (if (= side 'w) (cons 'E 1) (cons 'E 8)))
    (define midway (if (= side 'w)
                       (list (cons 'F 1) (cons 'G 1))
                       (list (cons 'F 8) (cons 'G 8))))
    (define rook-xy-end (if (= side 'w) (cons 'F 1) (cons 'F 8)))
    (define king-xy-end (if (= side 'w) (cons 'G 1) (cons 'G 8)))
    (imp-castling-generic! chessboard side rook-xy king-xy midway rook-xy-end king-xy-end))

(define (imp-ooo! chessboard side)
    ; the rook and king should not have been moved, that should have been
    ; checked before calling this function
    (define rook-xy (if (= side 'w) (cons 'A 1) (cons 'A 8)))
    (define king-xy (if (= side 'w) (cons 'E 1) (cons 'E 8)))
    (define midway (if (= side 'w)
                       (list (cons 'B 1) (cons 'C 1) (cons 'D 1))
                       (list (cons 'B 8) (cons 'C 8) (cons 'D 8))))
    (define rook-xy-end (if (= side 'w) (cons 'D 1) (cons 'D 8)))
    (define king-xy-end (if (= side 'w) (cons 'C 1) (cons 'C 8)))
    (imp-castling-generic! chessboard side rook-xy king-xy midway rook-xy-end king-xy-end))

(define (imp-castling-generic! chessboard side rook-xy king-xy midway rook-xy-end king-xy-end)
    (cond ; first, rook and king should not be attacked
          [(position-attacked? chessboard
                               (opponent-side side)
                               (car rook-xy)
                               (cdr rook-xy))
           (begin (display "Cannot castle, rook is attacked")
                  false)]
          [(position-attacked? chessboard
                               (opponent-side side)
                               (car king-xy)
                               (cdr king-xy))
           (begin (display "Cannot castle, king is attacked")
                  false)]
          ; then, midway should be all empty, and not attacked
          [(list-empty-not-attacked? chessboard side midway)
           (begin
               (chessboard-set! chessboard (car rook-xy) (cdr rook-xy) '())
               (chessboard-set! chessboard (car king-xy) (cdr king-xy) '())
               (chessboard-set! chessboard (car rook-xy-end) (cdr rook-xy-end) (if (= side 'w) 'R 'r))
               (chessboard-set! chessboard (car king-xy-end) (cdr king-xy-end) (if (= side 'w) 'K 'k))
               true)]
          [else (begin (display "Cannot castle, midway is not empty or attacked")
                       false)]))

(define (list-empty-not-attacked? chessboard side list)
    (if (= '() list)
        true
        (begin
            (define xy (car list))
            (define x (car xy))
            (define y (cdr xy))
            (define cell (chessboard-ref chessboard x y))
            (cond [(not (= cell '())) false]
                  [(position-attacked? chessboard
                                       (opponent-side side)
                                       x
                                       y)
                    false]
                  [else (list-empty-not-attacked? chessboard side (cdr list))]))))