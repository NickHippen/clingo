a(1). b(2). b(3). b(4).
{ a(X) } :- b(X).
c(X) :- a(X).
:- a(X), a(X+1).
:- a(X), not a(X+1), not a(X+2), a(X+3).