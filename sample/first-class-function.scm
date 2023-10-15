; 烈火卷雄风，红云映碧空
; 莽原好驰骋，风云天边涌
; 骐骥有良种，宝马待英雄
; 长驱疾如电，真堪托死生

(define (fst x y) x)
(define (snd x y) y)

(define (my-cons x y)
  (lambda (m) (m x y)))

(define (my-car z) (z fst))
(define (my-cdr z) (z snd))

(define a (my-cons 1 2))
(define b (my-cons "Zdravstvuyte" 'mir))

(display (my-car a) (my-cdr a))
(display (my-car b) "," (my-cdr b))
