#include <iostream>
#include "Myfunc.h"
using namespace std;
void printInt(void *arg){
        long long tmp = (long long)arg;
        cout << "arg is " << tmp << "func is "  << __func__ << endl;
}

