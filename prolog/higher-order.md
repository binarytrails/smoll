# Higher-Order Programming

Higher-order programming usually entails functions which take parameters as usual, but also
take in functions as parameters as well. The functions they take in can either be predicates,
in other words something that yields a Boolean, or may as well perform changes to the given
data.

For example, consider a list of elements L together with a function, f, that we wish to apply
to all the elements of L, both of which are supplied as arguments to a higher order function
mutate: mutate(f, L) → L0 where L0 is the list returned by mutate.

As a more concrete example, let L = [1, 2, 3, 4, 5], and let add one be a function that we
wish to apply to the elements of L: mutate(add one, [1, 2, 3, 4, 5]) → [2, 3, 4, 5, 6].

Note that mutate returns a list of the same size as the given list. To achieve something
similar in Prolog, we need to take a look at some special syntax. The special syntax is
Operation =.. [opname, arg1, arg2,...]. That is, we define an Operation as a list.

The head of the list is the rule we wish to execute (which we pass as an atom). The rest
of the elements in the list are the arguments that the rule would take. 
Note also the use of call to execute the operation, and that the variables
get bound respectively.

## Easy rules

Define the following:

1. minus(X,Y,Z) Evaluates to Z = X - Y.
2. plus(X,Y,Z) Evaluates to Z = X + Y.
3. times(X,Y,Z) Evaluates to Z = X * Y.
4. do op(OP,X,Y,Z) OP is a function/rule, which is any of the above. Given that rule,
apply it, and return Z as the result of that evaluation.
5. eval ops(X, Y, Z) only using do op to call the above rules in the current body, do
the following: Subtract Y from X and store value as A. Add X to A, and store the
value as B. Multiply B by X and return the value.

## Harder rules

Define the following:

1. mutate(OP, L1, L2) Given an operation OP, and a list L1, perform that operation OP
on each element of L1 to yield a list L2.
Recommendation: define a rule addOne(X, Y), which yields Y = X + 1, and invoke
the rule as: mutate(addOne, [1,2,3,4], L).

2. mutate if(P, OP, L1, L2) Same as above, but only apply OP if P (X) yields true
for the current element of L1. For example addOne if the current element is odd:
mutate if (isOdd, addOne, [1, 2, 3, 4, 5]) → [2, 2, 4, 4, 6].

