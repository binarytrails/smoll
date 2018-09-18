#include <iostream>

using namespace std;

void print(int v1, int v2, int &p1, int &p2)
{
    cout << "v1  = " << v1  << "\n" <<
            "v2  = " << v2  << "\n" <<
            "*p1 = " << &p1 << "\n" <<
            "*p2 = " << &p2 << "\n" <<
            "p1  = " << p1  << "\n" <<
            "p2  = " << p2  << "\n" <<
            "------------------------" << endl;
}

void local_pointers()
{
    int v1  = 1;
    int v2  = 2;
    int *p1 = &v1;
    int *p2 = &v2;
    
    cout << "------------------------" << endl;
    print(v1, v2, *p1, *p2);

    // pointer assignment
    p2 = p1;
    // value assignment
    *p2 = *p1;

    print(v1, v2, *p1, *p2);

    // dagling pointer (used to point somewhere valid)
    // heap allocation
    p1 = new int;
    delete p1;
    // points nowhere now
    p1 = NULL;
    delete p1;
    // can initialize non class types, same result
    p1 = new int(3);

    print(v1, v2, *p1, *p2);

    // memory release done by end of scope
}

class SomeClass
{
    public:
        SomeClass();
};

// TODO p1 is a class, go out of scope and verify it is freed or not
//      local variables vs dynamic
void dynamic_pointers()
{
}

int main()
{
    local_pointers();
}
