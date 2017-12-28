#include <fstream>
#include <iostream>
using namespace std;

int main()
{
    ofstream out("1.txt", ios::binary);
    out << "12321";
    out.close();
    return 0;
}