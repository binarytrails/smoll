%Facts

class('Stack').
class('BoundedStack').
class('ResettableBoundedStack').
class('Stack2').
class('Stack3').

interface('Lockable').

extends('BoundedStack', 'Stack').
extends('ResettableBoundedStack', 'BoundedStack').
extends('Stack2', 'ResettableBoundedStack').
extends('Stack3', 'Stack2').

implements('BoundedStack', 'Lockable').

declares('Stack', [private, 'ArrayList<String>', elements]).
declares('Stack', [protected, int, top]).
declares('Stack3', [public, double, entropy]).
declares('Stack3', [public, 'String', label]).
declares('Stack3', [public, bool, ignoreFlag]).

refines('Stack', [public, void, push, ['String']]).
defines('Stack', [public, 'String', pop, [void]]).
defines('BoundedStack', [public, 'void', push, ['String']]).
defines('ResettableBoundedStack', [public, 'void', reset, [void]]).
defines('Stack2', [public, 'String', gget, [void]]).

defines('Stack3', [public, void, entropicPush, ['String']]).
defines('Stack3', [public, 'String', entropicPop, [void]]).
defines('Stack3', [public, 'String', entropicTop, [void]]).
defines('Stack3', [public, int, entropicCount, [void]]).
defines('Stack3', [public, 'String', toString, [void]]).

%Rules

%inheritance_chain
class_inheritance(Class, Father) :- class(Class), class(Father),
    extends(Class, Father).
class_inheritance(Class, Ancestor) :- extends(Class, Father),
    class_inheritance(Father, Ancestor).

inheritance_chain(Class, Chain) :- findall(Ancestor, class_inheritance(Class, Ancestor), Chain).

%interface
class_methods(Class, Method) :- defines(Class, Method); refines(Class, Method).
class_methods([Current|Inherited], Method) :- class_methods(Current, Method);
    class_methods(Inherited, Method).

interface(Class, Interface) :- inheritance_chain(Class, Chain),
    findall(Method, class_methods(Chain, Method), InheritedMethods),
    findall(Method, class_methods(Class, Method), ClassMethods),
    append(ClassMethods, InheritedMethods, Methods),
    list_to_set(Methods, Interface).

%provides_interface
class_descendence(Child, Class) :- class(Child), class(Class),
    extends(Child, Class).
class_descendence(Descendant, Class) :- extends(Child, Class),
    class_descendence(Descendant, Child).

inheritance_chain_down(Class, Chain) :- findall(Child, class_descendence(Child, Class), Chain).

provides_interface(Class, Interface, List) :- interface(Interface),
    implements(Class, Interface),
    inheritance_chain_down(Class, Chain),
    append([Class], Chain, List).

%is_bloated
is_bloated(Class) :- extends(Class, Father),
    findall(Method, class_methods(Class, Method), Methods),
    findall(Method, class_methods(Father, Method), FatherMethods),
    length(Methods, MethodsLenght),
    length(FatherMethods, FatherMethodsLenght),
    MethodsLenght > FatherMethodsLenght.

