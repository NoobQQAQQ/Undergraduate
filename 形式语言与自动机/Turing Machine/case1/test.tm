#Q = {q0,cmp1,head1,append2,reset2,cmp2,head2,append1,reset1,a0,a1,a2,a3,ha,r0,r1,r2,r3,r4,hr}

#S = {0}

#G = {0,_,T,r,u,e,F,a,l,s}

#q0 = q0

#B = _

#F = {ha}

#N = 3

; alternately using tape1/tape2 to calculate fibonacci number
; when calculation is done, compare the tape containing result with tape0

; q0: begin
q0 ___ ___ *** r0 ; empty string
q0 0__ 000 **r cmp1

; cmp1: compare tape0 and tape1
cmp1 _0_ _0_ l** r0
cmp1 ___ ___ l** a0
cmp1 00_ 00_ rr* cmp1
cmp1 0__ 0__ ll* head1

; head1: set tape0 and tape1 to head
head1 00_ 00_ ll* head1
head1 ___ ___ rr* append2

; append2: append tape1 to tape2
append2 00_ 000 *rr append2
append2 0__ 0__ **l reset2

; reset2: set tape2 to head
reset2 0_0 0_0 **l reset2
reset2 0__ 0__ **r cmp2

; cmp2: compare tape0 and tape2
cmp2 __0 __0 l** r0
cmp2 ___ ___ l** a0
cmp2 0_0 0_0 r*r cmp2
cmp2 0__ 0__ l*l head2

; head2: set tape0 and tape2 to head
head2 0_0 0_0 l*l head2
head2 ___ ___ r*r append1

; append1: append tape2 to tape1
append1 0_0 000 *rr append1
append1 0__ 0__ *l* reset1

; reset1: set tape1 to head
reset1 00_ 00_ *l* reset1
reset1 0__ 0__ *r* cmp1

; a0: accept and clear tape0
; r0: reject and clear tape0
a0 0__ ___ l** a0
r0 0__ ___ l** r0

; write result on tape0
a0 ___ T__ r** a1
a1 ___ r__ r** a2
a2 ___ u__ r** a3
a3 ___ e__ *** ha

r0 ___ F__ r** r1
r1 ___ a__ r** r2
r2 ___ l__ r** r3
r3 ___ s__ r** r4
r4 ___ e__ *** hr