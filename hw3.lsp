;
; CS161 Hw3: Sokoban
; 
; *********************
;    READ THIS FIRST
; ********************* 
;
; All functions that you need to modify are marked with 'EXERCISE' in their header comments.
; Do not modify a-star.lsp.
; This file also contains many helper functions. You may call any of them in your functions.
;
; *Warning*: The provided A* code only supports the maximum cost of 4999 for any node.
; That is f(n)=g(n)+h(n) < 5000. So, be careful when you write your heuristic functions.
; Do not make them return anything too large.
;
; For Allegro Common Lisp users: The free version of Allegro puts a limit on memory.
; So, it may crash on some hard sokoban problems and there is no easy fix (unless you buy 
; Allegro). 
; Of course, other versions of Lisp may also crash if the problem is too hard, but the amount
; of memory available will be relatively more relaxed.
; Improving the quality of the heuristic will mitigate this problem, as it will allow A* to
; solve hard problems with fewer node expansions.
; 
; In either case, this limitation should not significantly affect your grade.
; 
; Remember that most functions are not graded on efficiency (only correctness).
; Efficiency can only influence your heuristic performance in the competition (which will
; affect your score).
;  
;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; General utility functions
; They are not necessary for this homework.
; Use/modify them for your own convenience.
;

;
; For reloading modified code.
; I found this easier than typing (load "filename") every time. 
;
(defun reload()
  (load "hw3.lsp")
  )

;
; For loading a-star.lsp.
;
(defun load-a-star()
  (load "a-star.lsp"))

;
; Reloads hw3.lsp and a-star.lsp
;
(defun reload-all()
  (reload)
  (load-a-star)
  (loadtests)
  )
  
(defun loadtests()
  (load "test-states.lsp")
)

;
; A shortcut function.
; goal-test and next-states stay the same throughout the assignment.
; So, you can just call (sokoban <init-state> #'<heuristic-name>).
; 
;
(defun sokoban (s h)
  (a* s #'goal-test #'next-states h)
  )

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; end general utility functions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; We now begin actual Sokoban code
;

; Define some global variables
(setq blank 0)
(setq wall 1)
(setq box 2)
(setq keeper 3)
(setq star 4)
(setq boxstar 5)
(setq keeperstar 6)

; Some helper functions for checking the content of a square
(defun isBlank (v)
  (= v blank)
  )

(defun isWall (v)
  (= v wall)
  )

(defun isBox (v)
  (= v box)
  )

(defun isKeeper (v)
  (= v keeper)
  )

(defun isStar (v)
  (= v star)
  )

(defun isBoxStar (v)
  (= v boxstar)
  )

(defun isKeeperStar (v)
  (= v keeperstar)
  )

;
; Helper function of getKeeperPosition
;
(defun getKeeperColumn (r col)
  (if (null r)
    nil
	(or (position keeper r) (position keeperstar r))
  )


  ;(cond ((null r) nil)
	;(t (if (or (isKeeper (car r)) (isKeeperStar (car r)))
	       ;col
	     ;(getKeeperColumn (cdr r) (+ col 1))
	     ;);end if
	   ;);end t
	;);end cond
  )

;
; getKeeperPosition (s firstRow)
; Returns a list indicating the position of the keeper (c r).
; 
; Assumes that the keeper is in row >= firstRow.
; The top row is the zeroth row.
; The first (right) column is the zeroth column.
; STUDENT COMMENT: isn't the leftmost column the zeroth column?
(defun getKeeperPosition (s row)
  (cond ((null s) nil)
	(t (let ((x (getKeeperColumn (car s) 0)))
	     (if x
		 ;keeper is in this row
		 (list x row)
		 ;otherwise move on
		 (getKeeperPosition (cdr s) (+ row 1))
		 );end if
	       );end let
	 );end t
	);end cond
  );end defun

;
; cleanUpList (l)
; returns l with any NIL element removed.
; For example, if l is '(1 2 NIL 3 NIL), returns '(1 2 3).
;
(defun cleanUpList (L)
  (cond ((null L) nil)
	(t (let ((cur (car L))
		 (res (cleanUpList (cdr L)))
		 )
	     (if cur 
		 (cons cur res)
		  res
		 )
	     );end let
	   );end t
	);end cond
  );end 

; EXERCISE: Modify this function to return true (t)
; if and only if s is a goal state of the game.
; (neither any boxes nor the keeper is on a non-goal square)
;
; Currently, it always returns NIL. If A* is called with
; this function as the goal testing function, A* will never
; terminate until the whole search space is exhausted.
;
(defun goal-test (s)
  (cond ((null s) t)             ; no misplaced boxes and keeper is on goal
    (t (if (goalTestCol (car s) 0)
		 nil                   ;box/keeper is in this row
		 (goal-test (cdr s))   ;next move on
	   );end if
	);end t
  );end cond
);end defun
  
; helper function for goal-test
; returns true if there's a box or keeper (not star) in this row (check all cols of row)
; based off getKeeperColumn
(defun goalTestCol (r col)
  (cond ((null r) nil)
	    (t (if (or (isBox (car r)) (isKeeper (car r)))
	         t
	         (goalTestCol (cdr r) (+ col 1))
	       );end if
	    );end t
  );end cond
)

; EXERCISE: Modify this function to return the list of 
; successor states of s.
;
; This is the top-level next-states (successor) function.
; Some skeleton code is provided below.
; You may delete them totally, depending on your approach.
; 
; If you want to use it, you will need to set 'result' to be 
; the set of states after moving the keeper in each of the 4 directions.
; A pseudo-code for this is:
; 
; ...
; (result (list (try-move s UP) (try-move s DOWN) (try-move s LEFT) (try-move s RIGHT)))
; ...
; 
; You will need to define the function try-move and decide how to represent UP,DOWN,LEFT,RIGHT.
; Any NIL result returned from try-move can be removed by cleanUpList.
; 
;
(defun next-states (s)
  (let* ((pos (getKeeperPosition s 0))
	 (col (car pos))
	 (row (cadr pos))
	 ;col and row are now the coordinate of the keeper in s.
	 (result (list (try-move s row col 1)
	               (try-move s row col 2)
	               (try-move s row col 3)
	               (try-move s row col 4)))
	 )
    (cleanUpList result);end
   );end let
  );


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      USEFUL THINGS       ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; get coords of objects! this will be useful
; based of getKeeperPosition
(defun getObjCols (r col obj)
  (cond ((null r) nil)
	(t (if (= obj (car r))
	     (cons col (getObjCols (cdr r) (+ col 1) obj))
	     (getObjCols (cdr r) (+ col 1) obj)
	   );end if
	);end t
  );end cond
)

; returns coords of all objects of type obj
; returns in format ((r c) (r c))
(defun getObjPos (s row obj)
  (cond ((null s) nil)
	(t (let ((x (getObjCols (car s) 0 obj)))
	     (if (not (null x))
		   (append (formatCoords row x) (getObjPos (cdr s) (+ row 1) obj))
		   (getObjPos (cdr s) (+ row 1) obj)
		 );end if
	   );end let
	);end t
  );end cond
);end defun


; helper for getObjPos, formatting
(defun formatCoords (r cols)
  (cond ((null cols) nil)
        (t (cons (list r (car cols)) (formatCoords r (cdr cols))))
  )
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;        TRY-MOVE IN EACH DIRECTION        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; just to make the function a bit cleaner without layers and layers of ifs

; helper function for next-states
; determines if a move is valid
(defun try-move (s r c dir)
  (cond ((= dir 1) (try-move-u s r c))     ; up 
        ((= dir 2) (try-move-d s r c))     ; down
		((= dir 3) (try-move-l s r c))     ; left 
		((= dir 4) (try-move-r s r c))     ; right
  )
)

; returns nil if keeper cannot move up, else returns updated state
; it's still pretty ugly...
(defun try-move-u (s r c)
  (let ((up (getValAtPos s (- r 1) c))      ; get obj at up position
	    (cur (getValAtPos s r c)))          ; get obj at cur position
	(cond ((null up) nil)        ; out of scope
	      ((and (not (isKeeper cur)) (not (isKeeperStar cur))) nil)
		                         ; not keeper
	      ((= up wall) nil)      ; blocked by wall
	      ((or (= up box) (= up boxstar))
	        (let ((new_state (try-move-u-box s (- r 1) c up)))
			  (if (null new_state)
			    nil
			   (let ((newup (move-on cur (getValAtPos new_state (- r 1) c)))
				     (newcur (move-off cur)))
		         (updateValAtPos (updateValAtPos new_state (- r 1) c newup) r c newcur)
		             ; this monstrosity... swap positions of cur and up
				)
		      )
		    )
		  )    ; if box is above, try to move the box
			   ; if box moves successfully, move yourself
		  (t 
			(let ((newup (move-on cur up))
				  (newcur (move-off cur)))
	          (updateValAtPos (updateValAtPos s (- r 1) c newup) r c newcur)
		    )
		  )    ; if nothing is above, move yourself
	)
  )
)

; returns nil if box cannot move up, else returns updated state
(defun try-move-u-box (s r c obj) 
  (let ((up (getValAtPos s (- r 1) c)))
    (cond ((null up) nil)       ; out of scope
	      ((and (not (isBox obj)) (not (isBoxStar obj))) nil)
		                        ; not box
	      ((= up wall) nil)     ; blocked by wall
	      ((or (= up box) (= up boxstar)) nil) ; blocked by another box
	  	  (t t                   ; not blocked
			(let* ((newup (move-on obj up))
			      (newcur (move-off obj)))
			  (updateValAtPos (updateValAtPos s (- r 1) c newup) r c newcur)
			); end let
		  )
	);end cond
  );end let
)

; returns nil if keeper cannot move down, else returns updated state
(defun try-move-d (s r c)
  (let ((down (getValAtPos s (+ r 1) c))      ; get obj at down position
	    (cur (getValAtPos s r c)))            ; get obj at cur position
	(cond ((null down) nil)        ; out of scope
	      ((and (not (isKeeper cur)) (not (isKeeperStar cur))) nil)
		                           ; not keeper
	      ((= down wall) nil)      ; blocked by wall
	      ((or (= down box) (= down boxstar))
	        (let ((new_state (try-move-d-box s (+ r 1) c down)))
			  (if (null new_state)
			    nil
			   (let ((newdown (move-on cur (getValAtPos new_state (+ r 1) c)))
				     (newcur (move-off cur)))
		         (updateValAtPos (updateValAtPos new_state (+ r 1) c newdown) r c newcur)
		             ; this monstrosity... swap positions of cur and down
				)
		      )
		    )
		  )    ; if box is above, try to move the box
			   ; if box moves successfully, move yourself
		  (t 
			(let ((newdown (move-on cur down))
				  (newcur (move-off cur)))
	          (updateValAtPos (updateValAtPos s (+ r 1) c newdown) r c newcur)
		    )
		  )    ; if nothing is above, move yourself
	)
  )
)

; returns nil if box cannot move down, else returns updated state
(defun try-move-d-box (s r c obj) 
  (let ((down (getValAtPos s (+ r 1) c)))
    (cond ((null down) nil)       ; out of scope
	      ((and (not (isBox obj)) (not (isBoxStar obj))) nil)
		                          ; not box
	      ((= down wall) nil)     ; blocked by wall
	      ((or (= down box) (= down boxstar)) nil) ; blocked by another box
	  	  (t t                   ; not blocked
			(let* ((newdown (move-on obj down))
			      (newcur (move-off obj)))
			  (updateValAtPos (updateValAtPos s (+ r 1) c newdown) r c newcur)
			); end let
		  )
	);end cond
  );end let
)

; returns nil if keeper cannot move left, else returns updated state
(defun try-move-l (s r c)
  (let ((left (getValAtPos s r (- c 1)))      ; get obj at left position
	    (cur (getValAtPos s r c)))            ; get obj at cur position
	(cond ((null left) nil)        ; out of scope
	      ((and (not (isKeeper cur)) (not (isKeeperStar cur))) nil)
		                           ; not keeper
	      ((= left wall) nil)      ; blocked by wall
	      ((or (= left box) (= left boxstar))
	        (let ((new_state (try-move-l-box s r (- c 1) left)))
			  (if (null new_state)
			    nil
			   (let ((newleft (move-on cur (getValAtPos new_state r (- c 1))))
				     (newcur (move-off cur)))
		         (updateValAtPos (updateValAtPos new_state r (- c 1) newleft) r c newcur)
		             ; this monstrosity... swap positions of cur and left
				)
		      )
		    )
		  )    ; if box is above, try to move the box
			   ; if box moves successfully, move yourself
		  (t 
			(let ((newleft (move-on cur left))
				  (newcur (move-off cur)))
	          (updateValAtPos (updateValAtPos s r (- c 1) newleft) r c newcur)
		    )
		  )    ; if nothing is above, move yourself
	)
  )
)

; returns nil if box cannot move left, else returns updated state
(defun try-move-l-box (s r c obj) 
  (let ((left (getValAtPos s r (- c 1))))
    (cond ((null left) nil)       ; out of scope
	      ((and (not (isBox obj)) (not (isBoxStar obj))) nil)
		                          ; not box
	      ((= left wall) nil)     ; blocked by wall
	      ((or (= left box) (= left boxstar)) nil) ; blocked by another box
	  	  (t t                   ; not blocked
			(let* ((newleft (move-on obj left))
			      (newcur (move-off obj)))
			  (updateValAtPos (updateValAtPos s r (- c 1) newleft) r c newcur)
			); end let
		  )
	);end cond
  );end let
)

; returns nil if keeper cannot move right, else returns updated state
(defun try-move-r (s r c)
  (let ((right (getValAtPos s r (+ c 1)))     ; get obj at right position
	    (cur (getValAtPos s r c)))            ; get obj at cur position
	(cond ((null right) nil)        ; out of scope
	      ((and (not (isKeeper cur)) (not (isKeeperStar cur))) nil)
		                            ; not keeper
	      ((= right wall) nil)      ; blocked by wall
	      ((or (= right box) (= right boxstar))
	        (let ((new_state (try-move-r-box s r (+ c 1) right)))
			  (if (null new_state)
			    nil
			   (let ((newright (move-on cur (getValAtPos new_state r (+ c 1))))
				     (newcur (move-off cur)))
		         (updateValAtPos (updateValAtPos new_state r (+ c 1) newright) r c newcur)
		             ; this monstrosity... swap positions of cur and right
				)
		      )
		    )
		  )    ; if box is above, try to move the box
			   ; if box moves successfully, move yourself
		  (t 
			(let ((newright (move-on cur right))
				  (newcur (move-off cur)))
	          (updateValAtPos (updateValAtPos s r (+ c 1) newright) r c newcur)
		    )
		  )    ; if nothing is above, move yourself
	)
  )
)

; returns nil if box cannot move left, else returns updated state
(defun try-move-r-box (s r c obj) 
  (let ((right (getValAtPos s r (+ c 1))))
    (cond ((null right) nil)       ; out of scope
	      ((and (not (isBox obj)) (not (isBoxStar obj))) nil)
		                           ; not box
	      ((= right wall) nil)     ; blocked by wall
	      ((or (= right box) (= right boxstar)) nil) ; blocked by another box
	  	  (t t                   ; not blocked
			(let* ((newright (move-on obj right))
			      (newcur (move-off obj)))
			  (updateValAtPos (updateValAtPos s r (+ c 1) newright) r c newcur)
			); end let
		  )
	);end cond
  );end let
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;        HELPER FUNCTIONS FOR THE HELPER FUNCTIONS        ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; gets the value at coordinates given a row and col
(defun getValAtPos (s r c) 
  (if (or (< r 0) (< c 0))
    nil
    (nth c (nth r s)) ; reached the row, get the column
  )
)


; replaces the character at coords (col, row) with the given character
(defun updateValAtPos (s r c char)
  (cond ((null s) nil)                       ; broken...
        ((= r 0) (cons (updateValAtPosCol (car s) c char) (cdr s)))
		    ; reached the row, get the column
	    (t (cons (car s) (updateValAtPos (cdr s) (- r 1) c char)))  ; go down one row
  )
)

; helper function for updateValAtPos
; given a row, replaces the cth character in the row with the given character
(defun updateValAtPosCol (row c char)
  (cond ((null row) nil)
        ((= c 0) (cons char (cdr row)))
		(t (cons (car row) (updateValAtPosCol (cdr row) (- c 1) char)))
  )
)

; returns the character to swap to when moving off this position
(defun move-off (mover)
  (cond ((or (isBox mover) (isKeeper mover)) blank)
		((or (isBoxStar mover) (isKeeperStar mover)) star)
  )
)

; returns the character to swap to when moving onto this position
(defun move-on (mover o)
  (cond ((and (isBlank o) (isBoxStar mover)) box)
        ((and (isBlank o) (isKeeperStar mover)) keeper)
        ((isBlank o) mover)
        ((and (or (isKeeperStar mover) (isKeeper mover)) (isStar o)) keeperstar)
        ((and (or (isBoxStar mover) (isBox mover)) (isStar o)) boxstar)
		(t nil)
  )
)

; EXERCISE: Modify this function to compute the trivial 
; admissible heuristic.
;
(defun h0 (s)
  0
)

; EXERCISE: Modify this function to compute the 
; number of misplaced boxes in s.
; ANSWER:
; This is an admissible heuristic. Each move, a player can move at most 1 box. 
; If there are n misplaced boxes, it will take at the very least n moves to get all
; of them onto the goal and to get the keeper onto the goal.
(defun h1 (s)
  (cond ((null s) 0)
	(t (+ (count box (car s)) (h1 (cdr s))));end t
  );end cond
)

; EXERCISE: Change the name of this function to h<UID> where
; <UID> is your actual student ID number. Then, modify this 
; function to compute an admissible heuristic value of s. 
; 
; This function will be entered in the competition.
; Objective: make A* solve problems as fast as possible.
; The Lisp 'time' function can be used to measure the 
; running time of a function call.

; backtracking + number of boxes + keeper! h1++
(defun h305110110 (s)
  (cond ((null s) 0)
    ((badState s (getObjPos s 0 box)) 1000)
	(t (h1 s));end t
  );end cond
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      BACKTRACK SEARCH ATTEMPT      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; some things i noticed while playing sokoban online
; never push a box into a corner unless there's a goal in the corner
; never have two boxes next to each other against the wall (unless goals)
; never form a four square of boxes (^ which is related to that one)
; don't push the box against the wall if the goal is in the opposite direction
; don't enclose a corner
; heavily weight those cases (or just the first three because coding is hard)

; return t if we should remove this state
(defun badState (s boxes)
  (cond ((null boxes) nil)
        (t (let* ((cur (car boxes))
		          (box_r (first cur))
				  (box_c (second cur)))
			 (or (boxStuck s box_r box_c) (badState s (cdr boxes)))
		   )
		)
  )
)


; for cleanliness
(defun isBlocking (obj)
  (or (null obj) (isBox obj) (isBoxStar obj) (isWall obj))
)

; given coordinates, returns t if box is in a corner
; or if box forms a square of boxes/walls/null
(defun boxStuck (s r c)
  (let ((up (getValAtPos s (- r 1) c))
	    (down (getValAtPos s (+ r 1) c))
	    (left (getValAtPos s r (- c 1)))
	    (right (getValAtPos s r (+ c 1)))
	    (ul (getValAtPos s (- r 1) (- c 1)))
	    (ur (getValAtPos s (- r 1) (+ c 1)))
	    (dl (getValAtPos s (+ r 1) (- c 1)))
	    (dr (getValAtPos s (+ r 1) (+ c 1))))
    (cond ((and (or (null up) (isWall up)) (or (null left) (isWall left))) t)
												 ; box in tl corner
		  ((and (or (null up) (isWall up)) (or (null right) (isWall right))) t)
												 ; box in tr corner
		  ((and (or (null down) (isWall down)) (or (null left) (isWall left))) t)
												 ; box in dl corner
		  ((and (or (null down) (isWall down)) (or (null right) (isWall right))) t)
												 ; box in dr corner
		  ((and (isBlocking down) (isBlocking right) (isBlocking dr)) t)
		                                         ; box in tl of 4 square
		  ((and (isBlocking down) (isBlocking left) (isBlocking dl)) t)
		                                         ; box in tr of 4 square
		  ((and (isBlocking up) (isBlocking right) (isBlocking ur)) t)
		                                         ; box in bl of 4 square
		  ((and (isBlocking up) (isBlocking left) (isBlocking ul)) t)
		                                         ; box in br of 4 square
		  (t nil)
    );end cond
  );end let
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      OTHER ATTEMPTS      ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; distance from keeper to nearest goal
; not really that good
(defun h305110110_keeperDist (s)
  (let* ((pos (car (getObjPos s 0 keeper))))
	(if (null pos)
	  0
	  (minDist (first pos) (second pos) (getObjPos s 0 star))
	)
  )
)

; min distance from box to nearest goal
(defun h305110110_minDist (s)
  (let* ((b (getObjPos s 0 box))
         (g (getObjPos s 0 star)))
    (minMoveDist s b g)
  )
)

; total move distance to get from each item in li to their nearest goal
(defun minMoveDist (s l1 l2)
  (cond ((null l1) 0)
        (t (let* ((cur (car l1)))
			 (+ (minDist (first cur) (second cur) l2)
 			    (minMoveDist s (cdr l1) l2))
		   )
		)
  )
)

; list the min distance between a (r, c) pos and an item in the list
(defun minDist (r c li)
  (cond ((null li) 0)
        ((null (cdr li))
		  (let* ((cur (car li)))
			(+ (abs (- r (first cur))) (abs (- c (second cur))))
		  )
		)
        (t (let* ((cur (car li)))
		     (min (+ (abs (- r (first cur))) (abs (- c (second cur)))) 
			       (minDist r c (cdr li)))
		   )
		)
  )
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#|
 | Some predefined problems.
 | Each problem can be visualized by calling (printstate <problem>). For example, (printstate p1).
 | Problems are ordered roughly by their difficulties.
 | For most problems, we also provide a number which indicates the depth of the optimal solution.
 | These numbers are located at the comments of the problems. For example, the first problem below has optimal solution depth 6.
 | As for the solution depth, any admissible heuristic must make A* return an optimal solution. So, the depths of the optimal solutions provided could be used for checking whether your heuristic is admissible.
 |
 | Warning: some problems toward the end are quite hard and could be impossible to solve without a good heuristic!
 | 
 |#
;(6)
(setq p1 '((1 1 1 1 1 1)
	   (1 0 3 0 0 1)
	   (1 0 2 0 0 1)
	   (1 1 0 1 1 1)
	   (1 0 0 0 0 1)
	   (1 0 4 0 4 1)
	   (1 1 1 1 1 1)))

;(15)
(setq p2 '((1 1 1 1 1 1 1)
	   (1 0 0 0 0 0 1) 
	   (1 0 0 0 0 0 1) 
	   (1 0 0 2 1 4 1) 
	   (1 3 4 0 1 0 1)
	   (1 1 1 1 1 1 1)))

;(13)
(setq p3 '((1 1 1 1 1 1 1 1 1)
	   (1 0 0 0 1 0 0 0 1)
	   (1 0 0 0 2 0 3 4 1)
	   (1 0 0 0 1 0 0 0 1)
	   (1 0 4 0 1 0 0 0 1)
	   (1 1 1 1 1 1 1 1 1)))

;(17)
(setq p4 '((1 1 1 1 1 1 1)
	   (0 0 0 0 0 1 4)
	   (0 0 0 0 0 0 0)
	   (0 0 1 1 1 0 0)
	   (0 0 1 0 0 0 0)
	   (0 2 1 0 0 4 0)
	   (0 3 1 0 0 0 0)))

;(12)
(setq p5 '((1 1 1 1 1 1)
	   (1 1 0 0 1 1)
	   (1 0 0 0 0 1)
	   (1 4 2 2 4 1)
	   (1 0 0 0 4 1)
	   (1 1 3 1 1 1)
	   (1 1 1 1 1 1)))

;(13)
(setq p6 '((1 1 1 1 1 1 1 1)
	   (1 0 0 0 0 0 4 1)
	   (1 4 0 0 2 2 3 1)
	   (1 0 0 1 0 0 4 1)
	   (1 1 1 1 1 1 1 1)))

;(47)
(setq p7 '((1 1 1 1 1 1 1 1 1 1)
	   (0 0 1 1 1 1 4 0 0 3)
	   (0 0 0 0 0 1 0 0 0 0)
	   (0 0 0 0 0 1 0 0 1 0)
	   (0 0 1 0 0 1 0 0 1 0)
	   (0 2 1 0 0 0 0 0 1 0)
	   (0 0 1 0 0 0 0 0 1 4)))

;(22)
(setq p8 '((1 1 1 1 1 1)
	   (1 4 0 0 4 1)
	   (1 0 2 2 0 1)
	   (1 2 0 1 0 1)
	   (1 3 4 0 4 1)
	   (1 1 1 1 1 1)))

;(34)
(setq p9 '((1 1 1 1 1 1 1 1 1) 
	   (1 1 1 0 0 1 1 1 1) 
	   (1 0 0 0 0 0 2 0 1) 
	   (1 0 1 0 0 1 2 0 1) 
	   (1 0 4 4 4 1 3 0 1) 
	   (1 1 1 1 1 1 1 1 1)))

;(59)
(setq p10 '((1 1 1 1 1 0 0)
	    (1 4 0 0 1 1 0)
	    (1 3 2 0 0 1 1)
	    (1 1 0 2 0 0 1)
	    (0 1 1 0 2 0 1)
	    (0 0 1 1 0 0 1)
	    (0 0 0 1 1 4 1)
	    (0 0 0 0 1 4 1)
	    (0 0 0 0 1 4 1)
	    (0 0 0 0 1 1 1)))

; h1 got (51)
(setq p11 '((0 0 1 0 0 0 0)
	    (0 2 1 4 0 4 0)
	    (0 2 0 4 0 0 0)	   
	    (3 2 1 1 1 4 0)
	    (0 0 1 4 0 0 0)))

; h1 got (41)
(setq p12 '((1 1 1 1 1 0 0 0)
	    (1 0 0 4 1 0 0 0)
	    (1 2 1 0 1 1 1 1)
	    (1 4 0 0 0 0 0 1)
	    (1 0 0 5 0 5 0 1)
	    (1 0 5 0 1 0 1 1)
	    (1 1 1 0 3 0 1 0)
	    (0 0 1 1 1 1 1 0)))

; h1 got (78)
(setq p13 '((1 1 1 1 1 1 1 1 1 1)
	    (1 3 0 0 1 0 0 4 4 1)
	    (1 0 2 0 2 0 0 4 4 1)
	    (1 0 2 2 2 1 1 4 4 1)
	    (1 0 0 0 0 1 1 4 4 1)
	    (1 1 1 1 1 1 0 0 0 0)))

;h1 got (26)
(setq p14 '((0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1)
	    (0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0)
	    (0 0 0 0 0 0 1 0 0 1 0 0 0 0 0 0)
	    (0 0 0 0 0 0 0 0 3 0 0 0 0 0 0 0)
	    (0 0 0 0 0 0 1 0 0 1 0 0 0 0 0 0)
	    (0 0 0 0 0 1 0 0 0 0 1 0 0 0 0 0)
	    (1 1 1 1 1 0 0 0 0 0 0 1 1 1 1 1)
	    (0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0)
	    (0 0 0 0 1 0 0 0 0 0 4 1 0 0 0 0)
	    (0 0 0 0 1 0 2 0 0 0 4 1 0 0 0 0)	    
	    (0 0 0 0 1 0 2 0 0 0 4 1 0 0 0 0)
	    ))

; ??
(setq p15 '((0 0 1 1 1 1 1 1 1 0)
	    (1 1 1 0 0 1 1 1 1 0)
	    (1 0 0 2 0 0 0 1 1 0)
	    (1 3 2 0 2 0 0 0 1 0)
	    (1 1 0 2 0 2 0 0 1 0)
	    (0 1 1 0 2 0 2 0 1 0)
	    (0 0 1 1 0 2 4 0 1 0)
	    (0 0 0 1 1 1 1 0 1 0)
	    (0 0 0 0 1 4 1 0 0 1)
	    (0 0 0 0 1 4 4 4 0 1)
	    (0 0 0 0 1 0 1 4 0 1)
	    (0 0 0 0 1 4 4 4 0 1)
	    (0 0 0 0 1 1 1 1 1 1)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

#|
 | Utility functions for printing states and moves.
 | You do not need to understand any of the functions below this point.
 |#

;
; Helper function of prettyMoves
; from s1 --> s2
;
(defun detectDiff (s1 s2)
  (let* ((k1 (getKeeperPosition s1 0))
	 (k2 (getKeeperPosition s2 0))
	 (deltaX (- (car k2) (car k1)))
	 (deltaY (- (cadr k2) (cadr k1)))
	 )
    (cond ((= deltaX 0) (if (> deltaY 0) 'DOWN 'UP))
	  (t (if (> deltaX 0) 'RIGHT 'LEFT))
	  );end cond
    );end let
  );end defun

;
; Translates a list of states into a list of moves.
; Usage: (prettyMoves (a* <problem> #'goal-test #'next-states #'heuristic))
;
(defun prettyMoves (m)
  (cond ((null m) nil)
	((= 1 (length m)) (list 'END))
	(t (cons (detectDiff (car m) (cadr m)) (prettyMoves (cdr m))))
	);end cond
  );

;
; Print the content of the square to stdout.
;
(defun printSquare (s)
  (cond ((= s blank) (format t " "))
	((= s wall) (format t "#"))
	((= s box) (format t "$"))
	((= s keeper) (format t "@"))
	((= s star) (format t "."))
	((= s boxstar) (format t "*"))
	((= s keeperstar) (format t "+"))
	(t (format t "|"))
	);end cond
  )

;
; Print a row
;
(defun printRow (r)
  (dolist (cur r)
    (printSquare cur)    
    )
  );

;
; Print a state
;
(defun printState (s)
  (progn    
    (dolist (cur s)
      (printRow cur)
      (format t "~%")
      )
    );end progn
  )

;
; Print a list of states with delay.
;
(defun printStates (sl delay)
  (dolist (cur sl)
    (printState cur)
    (sleep delay)
    );end dolist
  );end defun
