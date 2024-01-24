#include <iostream>
#include "Test.h"
#include "MpTerm.h"

void printHelp() {
    std::cout << "1 - Start konzole s neomezenou presnosti cisla." << std::endl;
    std::cout << "2 - Start konzole s omezenou presnosti cisla na 32-bitu." << std::endl;
    std::cout << "3 - Showcase vyuziti knihovny pro praci s neomezenymy cisly." << std::endl;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Program potrebuje prave jeden argument [1|2|3]." << std::endl;
        printHelp();
        return EXIT_FAILURE;
    }
    std::string argument(argv[1]);
    if (argument == "1") {
        MpTerm<MP_INT_UNLIMITED>().run();
        return EXIT_SUCCESS;
    } else if (argument == "2") {
        MpTerm<4>().run();
        return EXIT_SUCCESS;
    } else if (argument == "3") {
        test();
        return EXIT_SUCCESS;
    } else {
        printHelp();
        return EXIT_FAILURE;
    }
}