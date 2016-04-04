# Introduction

You are given a declarative database whose statements correspond to facts about a Java
program. The structure of the facts are as follows:

1. class(Class).
2. interface(Interface).
3. extends(Class1, Class2) : Class1 subclassifies Class2.
4. implements(Class, Interface): Class implements Interface.
5. declares(Class, [modifier, type, name]) : Class declares an attribute.
6. defines(Class, [modifier, return-type, name, [parameter-list]]) : Class defines a method.

Your mission is to extend this database with rules as follows:

1. interface(?Class, ?Interface) Succeeds by obtaining a set whose elements are lists
corresponding to the methods that ?Class makes available as part of ?Interface.
2. inheritance chain(?Class, ?Chain) Succeeds by obtaining a list containing the inheritance chain of ?Class.
3. provides interface(?Class, ?Interface, ?List) Succeeds by obtaining a list containing all classes that support ?Interface.
4. is bloated(?Class) Succeeds if ?Class introduces more features than its root class.

Your rule engine should work with any database whose structure corresponds to the one
given.

