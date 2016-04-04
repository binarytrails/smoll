%easy

plus2(X, Y, Z) :- Z is (X + Y).

minus(X, Y, Z) :- Z is (X - Y).

times(X, Y, Z) :- Z is (X * Y).

do_op(OP, X, Y, Z) :- call(OP, X, Y, Z).

eval_op(X, Y, Z) :- do_op(minus, X, Y, A),
    do_op(plus2, X, A, B),
    do_op(times, X, B, Z).

%harder

add_one(X, Y) :- Y is (X + 1).

is_even(X) :- 0 is X mod 2.
is_odd(X) :- not(is_even(X)).

mutate_list(OP, [L1H|L1T], MutatedElement) :- call(OP, L1H, MutatedElement);
    mutate_list(OP, L1T, MutatedElement).

mutate(OP, L1, L2) :- findall(MutatedElement, mutate_list(OP, L1, MutatedElement), L2). 

mutate_list_if(P, OP, [L1H|L1T], MutatedElement) :- call(P, L1H) -> 
    (
        call(OP, L1H, MutatedElement);
        mutate_list_if(P, OP, L1T, MutatedElement)
    );(
        MutatedElement is L1H;
        mutate_list_if(P, OP, L1T, MutatedElement)
    ).

mutate_if(P, OP, L1, L2) :- findall(MutatedElement, mutate_list_if(P, OP, L1, MutatedElement), L2).

