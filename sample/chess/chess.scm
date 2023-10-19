(define (imp-move! chessboard side piece x y)
    (error "unimplemented yet"))

(define (imp-oo! chessboard side)
    (error "unimplemented yet"))

(define (imp-ooo! chessboard side)
    (error "unimplemented yet"))


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
