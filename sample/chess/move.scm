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
