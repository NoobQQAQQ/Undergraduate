#Q = {q0,cp,reset,right,mark1,left,mark2,head0,cmp,a0,a1,a2,a3,ha,r0,r1,r2,r3,r4,hr}

#S = {0,1}

#G = {0,1,X,_,T,r,u,e,F,a,l,s}

#q0 = q0

#B = _

#F = {ha}

#N = 3

; use tape1 and mark X to find the middle position of the input
; alternately scan tape1 left/right to mark X on it
; when middle is found, compare tape0/tape2 to judge whether the input is of form ww

; q0: begin
q0 ___ ___ *** a0 ; empty string
q0 0__ 0_0 rrr cp
q0 1__ 1_1 rrr cp

; cp: copy input from tape0 to tape2
cp 0__ 0_0 rrr cp
cp 1__ 1_1 rrr cp
cp ___ _X_ lll reset

; reset: set tape1 and tape2 to head
reset *_* *_* *ll reset
reset *__ *X_ *rr right

; right: scan tape1 from left to right to find X
right *** *** *r* right
right *X* *X* *l* mark1

; mark1: judge whether to mark X and scan to left or start compare
mark1 *_* _X* ll* left
mark1 *X* *X* l** head0

; left: similar with right
left *** *** *l* left
left *X* *X* *r* mark2

: mark2: similar with mark1
mark2 *_* *X* *rr right
mark2 *X* *X* l** head0

; head0: set tape0 to head
head0 *X* *X* l** head0
head0 _X* _X* r** cmp

; cmp: compare the first half of tape0 and the last half of tape2
cmp 0X0 _X0 r*r cmp
cmp 1X1 _X1 r*r cmp
cmp _X_ _X_ *** a0
cmp _X* _X* r** r0
cmp *X_ _X* r** r0
cmp 0X1 _X1 r** r0
cmp 1X0 _X0 r** r0

; a0~a3, ha: accept
a0 _** T** r** a1
a1 _** r** r** a2
a2 _** u** r** a3
a3 _** e** *** ha

; r0: reject and clear tape0
r0 *** _** r** r0
r0 _** F** r** r1
r1 _** a** r** r2
r2 _** l** r** r3
r3 _** s** r** r4
r4 _** e** *** hr