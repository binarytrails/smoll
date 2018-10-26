// Vsevolod Ivanov

#include <iostream>

using namespace std;

int main()
{
    const int width = 2;
    const int height = 3;

    // static
    int array[2][3] = {1, 2, 3, 4, 5, 6};

    cout << array + 1 << endl;
    cout << *(array + 1) << endl;
    cout << *(*(array + 1)) << endl;

    for (int i = 0; i < width; ++i)
        for (int j = 0; j < height; ++j)
            cout << *(*(array + i) + j) << " ";
    cout << endl;

    // dynamic
    int** array2 = new int*[width];
    for (int i = 0; i < width; ++i)
        array2[i] = new int[height];

    for (int i = 0; i < width; ++i)
        for (int j = 0; j < height; ++j)
            // 0, 1, 2, 1, 2, 3
            array2[i][j] = i + j;

    for (int i = 0; i < width; ++i)
        for (int j = 0; j < height; ++j)
            cout << *(*(array2 + i) + j) << " ";
    cout << endl;

    for (int i = 0; i < width; ++i)
        delete [] array2[i];
    delete [] array2;
    array2 = NULL;
}
