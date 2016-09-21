#include <iostream>
using namespace std;

int main()
{
    cout << "1. Comparing array elements using * and &" << endl;
    int *a;
    a = new int[10];
    size_t a_s = sizeof(int);

    cout << "Remember: true is " << true << endl << endl;
    
    cout << a << endl;              // default is 1st element
    cout << &a[0] << endl;          // first element
    cout << &a[0] + a_s << endl;    // second element
    
    cout << endl << "Compared equal values are : " <<
        a[1] << *(&a[0] + a_s) << *(a + a_s) << endl << endl;
    
    // Fails with : v1 == v2 == v2
    cout << "Result is " <<
        (a[1] == *(&a[0] + a_s) == *(a + a_s)) << endl;
    
    // Works with : v1 == v2 && v1 == v3
    if ((a[1] == *(&a[0] + a_s)) && (a[1] == *(a + a_s)))
        cout << "Result is " << true << endl;
}
