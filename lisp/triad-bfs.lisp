(defun make-graph () (list '()))
; (make-graph)

(defun list-length2 (lst)
    (cond ((null lst) 0)
        ((null (cdr lst)) 1)
        (t (+ 1 (list-length2 (cdr lst))))
))

(defun is-triple(l)
    (cond ((= (list-length2 l) 3) t)
))
; is-triple '(a b c) ; -> T

(defun is-member (lst e)
    (cond ((null lst) nil) 
        ((equal (car lst) e) t)
        (t (is-member (cdr lst) e))
))
; is-member '(a b c d) 'd ; -> T

(defun triple-has-node (l e)
    (cond ((not (is-triple l)) nil)
        ((is-member l e) t) 
))
; triple-has-node '(a b c) 'a ; -> T

(defun push-unique (lst e)
    (cond ((null lst) nil)
        ((equal (car lst) e) lst)
        ((null (cdr lst)) (cons (car lst) (list e)))
        (t (cons (car lst) (push-unique (cdr lst) e)))
))
; push-unique '(a b c) 'd -> (a b c d)

(defun filter-visited (g e)
    (if (null g)
        nil
        (if (equal (car (car g)) e)
            (filter-visited (cdr g) e)
            (cons (car g) (filter-visited (cdr g) e))
        )
))
; filter-visited '((a 1 b) (b 2 c) (c 3 d)) 'a ; -> ((b 2 c) (c 3 d))

(defun filter-all-visited (g els)
    (cond ((null els) g)
        (t (filter-all-visited (filter-visited g (car els)) (cdr els)))
))
; filter-all-visited '((a 1 b) (b 2 c) (c 3 d)) '(a b) ; -> ((c 3 d))

(defun remove-from-lst (lst el)
    (cond ((null lst) nil)
        ((equal (car lst) el) (remove-from-lst (cdr lst) el))
        (t (cons (car lst) (remove-from-lst (cdr lst) el)))
))
; remove-from-lst (list 3 3 3 3) 3 ; -> nil
; remove-from-lst (list 3 3 1 3) 3 ; -> (1)

(defun remove-all-from-lst (ls1 ls2)
    (cond ((null ls2) ls1)
        (t (remove-all-from-lst (remove-from-lst ls1 (car ls2)) (cdr ls2)))
))
; remove-all-from-lst '(1 2 3 4) '(2 3) ; -> (1 4)

(defun triple-first (triple) (car triple))
; triple-first '(a 1 b) ; -> a

(defun triple-third (triple) (car (cdr (cdr triple))))
; triple-third '(a 1 b) ; -> b

(defun triple-nodes (g) (list (triple-first g) (triple-third g)))
;  triple-nodes '(a 1 b) ; -> (a b)

(defun triple-to-els (g)
    (cond ((null g) nil)
        (t (append (triple-nodes (car g)) (triple-to-els (cdr g))))
))
; triple-to-els '((a 1 b) (a 2 c)) ; -> (a b a c)

; breadth-first-search recursively

(defun level-nodes (g l)
    (cond ((null g) nil)
        ((equal (car (cdr (car g))) l) 
            (cons (car g) (level-nodes (cdr g) l)))
        (t (level-nodes (cdr g) l))
))
; level-nodes '((a 1 b) (b 2 c) (c 3 d) (b 2 e)) 2
;     -> ((b 2 c) (b 2 3))

(defun level (triple) (car (cdr triple)))

; bfs state: unfinished.
; todo: equal len(q) 1 -> cons & change c.
(defun bfs (g e q v c)
    (cond ; Base Cases :
        ; 1. not found (graph - visited = nil)
        ((null (filter-all-visited g v)) nil)
        ; 2. found (return c)
        ((equal c e) (c))
        ; Logic :
        ; null level -> change, find all, new c; recur
        ((null q)
            (bfs g e (level-nodes g (+ 1 (level c))) v c))
        ; else: pop one from q; flag c visited; give new c;
        ((not (null q))
            ; search
            (bfs g e (cdr q) (push-unique v c) (car q)))
))
; bfs '((a 1 b) (a 2 c) (c 4 d) (b 3 d) (d 5 e)) 'e '(a) '() 'ay
; -> (a b c d e)

; Remixed of 3.15 Example: Shortest Path from ANSI Common Lisp by Paul Graham 1996

(setf min ' ( ( a b c) (b c) (c d ) ) )

(defun bfs (g e q) ; e q g
    (cond
        ((null q) (nil)) ; not found
        (t  (let ((v (car q)))
                (let ((c (car v)))
                    (cond ((eql c e) (reverse v)) ; found
                        (t (bfs g e (append (cdr q) (new-paths v c g))))
                    )
                 )
             )
         )
     )
)

(defun new-paths (path node net)
    (mapcar #'(lambda (n) (cons n path))
    (cdr (assoc node net))
))

(defun shortest-path (start end net)
    (bfs net end (list (list start)))
)

; I will miss you Windows 2000.
