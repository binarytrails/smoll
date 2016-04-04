# Implementation

1. make-graph(): returns an empty list, which will be treated as a graph.

2. is-triple(l): takes in a list (ie: not an atom), and checks to see if the list is in fact a
triad. Returns true if that is the case. False (nil) if not.

        Input: (a, b, c)
        Output: True

        Input: (a, b)
        Output: False

        Input: ()
        Output: False

3. triple-has-node(l e): takes in a triple, and an element (atom). If triple contains the
node ‘e’, it will return true. False if not.

        Input: (a, 1, b) a
        Output: True

        Input: (b, 1, c) c
        Output: True

4. is-member(lst e): ‘lst’ is a list containing atoms. We check to see if ‘e’ exists in the
list. If exists, return true, else false.

        Input: (1, 2, 3, 4, 5) 4
        Output: True

6. push-unique(lst e): given a list ‘lst’ composed of atoms, we try to add ‘e’. If ‘e’ already
exists the original list is returned. If ‘e’ is not found, it is added at the end of the list
‘lst’.

        Input: (a, b, c) d
        Output: (a, b, c, d)

        Input: (a, b, c) c
        Output: (a, b, c)

6. filter-visited (g e): ‘g’ is the graph expressed as a list of triads. ‘e’ is a node. This
function goes through ‘g’, and will check to see if the triads contain the node. If a
triad contains the node, the triad is filtered out. In short this function will only return
the triads in a list, which do not contain ‘e’.

        Input: ((a, 1, b), (b, 2, c), (c, 3, d)) a
        Output: ((b, 2, c), (c, 3, d))

7. filter-all-visited (g els): same as filter-visited, but we provide a list of nodes (atoms),
and any occurence of those nodes should filter out triads from ‘g’. Hint: re-use filtervisited in this function.

        Input: ((a, 1, b), (c, 2, d), (e, 3, f ), (g, 4, h)) (e, g)
        Output: ((a, 1, b), (c, 2, d))

8. remove-from-lst (lst el): Removes all occurences of an element ‘el’ within a list ‘lst’.
Returns a list, with no occurences of ‘el’.

        Input: (3, 3, 3, 3) 3
        Output: ()

        Input: (3, 1, 3, 3, 3) 3
        Output: (1)

9. remove-all-from-lst (lst1 lst2): Given a list ‘lst2’ full of atoms, we check ‘lst1’, and
remove any elements matching those atoms.

        Input: (1, 2, 3, 4) (2, 3)
        Output: (1, 4)

10. triple-first (triple): Gives the first element of a triad.

        Input: (a, 1, b)
        Output: a

11. triple-third (triple): Gives the third element of a triad.

        Input: (a, 1, b)
        Output: b

12. triple-to-els (g): Given a list full of triads, extract all the nodes to a flat list.

        Input: ((a, 1, b), (a, 2, c))
        Output: (a, b, a, c)

## Breadth-first-search

    bfs(g e q v c):

Given a graph ‘g’ (composed of triads/triples), we look for an element
‘e’ within it. We have an accumulator to help us implement breadth-first-search recursively, which is named ‘q’ (you can think of this as the search queue). We keep track
of all the visited nodes, in another list ‘v’. We keep track of the current element we
are looking from in a variable ‘c’. Implement this recursively. Do not use setq. The
output you should return is the path the algorithm took, until reaching that element
‘e’. Pay close attention to the given input. This should give you strong hints on how
to implement your BFS algorithm.

    Input: ((a, 1, b), (a, 2, c), (c, 4, d), (b, 3, d), (d, 5, e)) e (a) () a
    Output: (a, b, c, d, e)

