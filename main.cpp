#include <iostream>
#include "MpInt.h"
#include "Test.h"

#define TESTING 1
#ifdef TESTING

int main() {
    test();
    return 0;
}

#else

int main() {
    std::cout << "Running terminal!" << std::endl;
    return 0;
}

#endif
