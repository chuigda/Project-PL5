(define c (make-chessboard))

; 后翼弃兵与阿尔宾反弃兵

; white move from D2 to D4
(imp-move! c 'w 'D 2 'D 4)
; black move from D7 to D5
(imp-move! c 'b 'D 7 'D 5)

(display (chessboard->string c))

; white move from C2 to C4 (queen's gambit)
(imp-move! c 'w 'C 2 'C 4)

; black move from E7 to E5
(imp-move! c 'b 'E 7 'E 5)

(display (chessboard->string c))

; white capture black pawn at E5
(imp-move! c 'w 'D 4 'E 5)

; then black could push pawn to D4
(imp-move! c 'b 'D 5 'D 4)

(display (chessboard->string c))

; white don't want black to push pawn at D lane, using E pawn to defend
(imp-move! c 'w 'E 2 'E 3)

; then, black could attack white king with bishop
(imp-move! c 'b 'F 8 'B 4)

(display (chessboard->string c))
