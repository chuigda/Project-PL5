(define drill-testboard (make-chessboard))

; white move from E2 to E4
(imp-move! drill-testboard
           'w
           'E 2
           'E 4)
; black move from E7 to E5
(imp-move! drill-testboard
           'b
           'E 7
           'E 5)

(display (chessboard->string drill-testboard))
