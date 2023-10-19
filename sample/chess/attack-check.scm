; return true if the coordinate is attacked by a piece of the given side
(define (position-attacked? chessboard side x y)
    (or-list (list (pawn-attack? chessboard side x y)
                   (king-attack? chessboard side x y)
                   (knight-attack? chessboard side x y)
                   (rook-attack? chessboard side x y)
                   (bishop-attack? chessboard side x y)
                   (queen-attack? chessboard side x y))))

; return true if attacker piece is found at any of the given positions
(define (has-attacker-at-position-list? chessboard x y positions attacker)
    (if (= positions '())
        false
        (if (has-attacker-at-position? chessboard x y (car positions) attacker)
            true
            (has-attacker-at-position-list? chessboard x y (cdr positions) attacker))))

; return true if attacker piece is found at the given position
(define (has-attacker-at-position? chessboard x y position attacker)
    (if (or (= (car position) '())
               (= (cdr position) '()))
        false
        (= (chessboard-ref chessboard (car position) (cdr position)) attacker))) 

; return true if attacker piece is found at (apply-n dx x) (apply-n dy y)
(define (has-attacker? chessboard x y dx dy attacker)
    (cond [(= x '()) false]
          [(= y '()) false]
          [else (begin
              (define this-piece (chessboard-ref chessboard x y))
              (cond [(= this-piece attacker) true]
                    [(= this-piece '())
                     (has-attacker? chessboard (dx x) (dy y) dx dy attacker)]
                    [else false]))]))

; return true if the coordinate is attacked by a pawn of the given side
(define (pawn-attack? chessboard side x y)
    (define pawn-piece (if (= side 'w) 'P 'p))
    (define pawn-y (if (= side 'w) (delimited- y) (delimited+ y)))
    (define pawn-x-left (alpha- x))
    (define pawn-x-right (alpha+ x))
    (has-attacker-at-position-list?
        chessboard
        x
        y
        (list (cons pawn-x-left pawn-y)
              (cons pawn-x-right pawn-y))
        pawn-piece))

; return true if the coordinate is attacked by a king of the given side
(define (king-attack? chessboard side x y)
    (define king-piece (if (= side 'w) 'K 'k))
    (define x-left (alpha- x))
    (define x-right (alpha+ x))
    (define y-forward (delimited- y))
    (define y-backward (delimited+ y))
    (has-attacker-at-position-list?
        chessboard
        x
        y
        (list (cons x-left y-forward)
              (cons x-left y)
              (cons x-left y-backward)
              (cons x y-forward)
              (cons x y-backward)
              (cons x-right y-forward)
              (cons x-right y)
              (cons x-right y-backward))
        king-piece))

; return true if the coordinate is attacked by a knight of the given side
(define (knight-attack? chessboard side x y)
    (define knight-piece (if (= side 'w) 'N 'n))
    (define x-left (alpha- x))
    (define x-right (alpha+ x))
    (define y-forward (delimited- y))
    (define y-backward (delimited+ y))
    (define x-left2 (alpha-2 x))
    (define x-right2 (alpha+2 x))
    (define y-forward2 (delimited-2 y))
    (define y-backward2 (delimited+2 y))
    (has-attacker-at-position-list?
        chessboard
        x
        y
        (list (cons x-left y-forward2)
              (cons x-left2 y-forward)
              (cons x-left2 y-backward)
              (cons x-left y-backward2)
              (cons x-right y-forward2)
              (cons x-right2 y-forward)
              (cons x-right2 y-backward)
              (cons x-right y-backward2))
        knight-piece))

; return true if there's an attacker at horizontal/vertical direction
(define (has-attacker-at-hv? chessboard x y attacker)
    (or-list (list (has-attacker? chessboard x y alpha- id attacker)
                   (has-attacker? chessboard x y alpha+ id attacker)
                   (has-attacker? chessboard x y id delimited- attacker)
                   (has-attacker? chessboard x y id delimited+ attacker))))

; return true if there's an attacker at diagonal direction
(define (has-attacker-at-diagonal? chessboard x y attacker)
    (or-list (list (has-attacker? chessboard x y alpha- delimited- attacker)
                   (has-attacker? chessboard x y alpha- delimited+ attacker)
                   (has-attacker? chessboard x y alpha+ delimited- attacker)
                   (has-attacker? chessboard x y alpha+ delimited+ attacker))))

; return true if the coordinate is attacked by a rook of the given side
(define (rook-attack? chessboard side x y)
    (define rook-piece (if (= side 'w) 'R 'r))
    (has-attacker-at-hv? chessboard x y rook-piece))

; return true if the coordinte is attacked by a bishop of the given side
(define (bishop-attack? chessboard side x y)
    (define bishop-piece (if (= side 'w) 'B 'b))
    (has-attacker-at-diagonal? chessboard x y bishop-piece))

; return true if the coordinate is attacked by a queen of the given side
(define (queen-attack? chessboard side x y)
    (define queen-piece (if (= side 'w) 'Q 'q))
    (or (has-attacker-at-hv? chessboard side x y queen-piece)
        (has-attacker-at-diagonal? chessboard side x y queen-piece)))
