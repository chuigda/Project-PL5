(define current-game '())

(define (new-game)
    (set! 'current-game
        (vector
            ; chessboard
            (make-chessboard)
            ; current side
            'w
            ; white can left or right castle
            (cons true true)
            ; black can left or right castle
            (cons true true)))
    (display (chessboard->string (vector-ref current-game 0))))

(define (move! start-x start-y end-x end-y)
    (define side (vector-ref current-game 1))
    (if (imp-move! (vector-ref current-game 0)
                    side
                    start-x
                    start-y
                    end-x
                    end-y)
        (begin
            (vector-set! current-game 1 (opponent-side side))
            ; if moved king, then both castles are not allowed
            (if (and (or (and (= start-y 1) (= side 'w))
                         (and (= start-y 8) (= side 'b)))
                     (= start-x 'E))
                (if (= side 'w)
                    (vector-set! current-game 2 (cons false false))
                    (vector-set! current-game 3 (cons false false))))
            ; if moved left rook, then left castle (long castling, o-o-o) is not allowed
            (if (and (or (and (= start-y 1) (= side 'w))
                         (and (= start-y 8) (= side 'b)))
                     (= start-x 'A))
                (if (= side 'w)
                    (set-car! (vector-ref current-game 2) false)
                    (set-car! (vector-ref current-game 3) false)))
            ; if moved right rook, then right castle (short castling, o-o) is not allowed
            (if (and (or (and (= start-y 1) (= side 'w))
                         (and (= start-y 8) (= side 'b)))
                     (= start-x 'H))
                (if (= side 'w)
                    (set-cdr! (vector-ref current-game 2) false)
                    (set-cdr! (vector-ref current-game 3) false)))))
    (display (chessboard->string (vector-ref current-game 0))))

(define (o-o!)
    (define side (vector-ref current-game 1))
    (define side-casting-info
        (if (= side 'w) (vector-ref current-game 2)
                        (vector-ref current-game 3)))
    (cond [(not (cdr side-casting-info)) (display "castling is not allowed")]
          [(imp-oo! (vector-ref current-game 0) side)
           (begin
               (vector-set! current-game 1 (opponent-side side))
               (if (= side 'w)
                   (vector-set! current-game 2 (cons false false))
                   (vector-set! current-game 3 (cons false false))))])
    (display (chessboard->string (vector-ref current-game 0))))

(define (o-o-o!)
    (define side (vector-ref current-game 1))
    (define side-casting-info
        (if (= side 'w) (vector-ref current-game 2)
                        (vector-ref current-game 3)))
    (cond [(not (car side-casting-info)) (display "castling is not allowed")]
          [(imp-ooo! (vector-ref current-game 0) side)
           (begin
               (vector-set! current-game 1 (opponent-side side))
               (if (= side 'w)
                   (vector-set! current-game 2 (cons false false))
                   (vector-set! current-game 3 (cons false false))))])
    (display (chessboard->string (vector-ref current-game 0))))
