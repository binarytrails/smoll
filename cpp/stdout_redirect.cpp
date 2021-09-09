// @author Vsevolod Ivanov <seva@binarytrails.net>

#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// method 1
// Uncomment to see precendence over set_cout_redirect
// inline overrides any lib calls to other overrides
/*
inline std::ostream& operator<<(std::ostream& s, const char* c) {
    printf("[override intercept] %s\n", c);
    return s;
}
*/

// method 2
streambuf* cout_buf;
ostringstream str_buf;
void set_cout_redirect()
{
    cout_buf = cout.rdbuf();
    cout.rdbuf(str_buf.rdbuf());
}

int main()
{
    set_cout_redirect(); // method 2

    cout << "Welcome into Main function!" << endl;

    if (!str_buf.str().empty()){ // method 2
        printf("[cout rdbuf intercept] %s", str_buf.str().c_str());
        str_buf.str(string());
    }
    return 0;
}
