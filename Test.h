
#include <iostream>
#include <limits>
#include <random>
#include "MpInt.h"

#undef COLORED

std::string printRight(const std::string &s) {
#ifdef COLORED
    return "\033[32m" + s + "\033[0m";
#else
    return s;
#endif
}

std::string printWrong(const std::string &s) {
#ifdef COLORED
    return "\033[31m" + s + "\033[0m";
#else
    return s;
#endif
}

std::string printInfo(const std::string &s) {
#ifdef COLORED
    return "\033[34m" + s + "\033[0m";
#else
    return s;
#endif
}
constexpr auto longLongMin = std::numeric_limits<long long>::min();
constexpr auto longLongMax = std::numeric_limits<long long>::max();

constexpr auto intMin = std::numeric_limits<int>::min();
constexpr auto intMax = std::numeric_limits<int>::max();

constexpr auto shortMin = std::numeric_limits<short>::min();
constexpr auto shortMax = std::numeric_limits<short>::max();

void testOverflow(std::size_t &testSuccess, std::size_t &testFailed) {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << printInfo("Overflow limits testing start") << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "+" "max + max" overflow test)") << std::endl;
    try {
        auto res = MpInt<8>(longLongMax) + MpInt<8>(longLongMax);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "18446744073709551614") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "+" "min + min" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMin) + MpInt<8>(longLongMin);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "-18446744073709551616") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "-" "max - min" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMax) - MpInt<8>(longLongMin);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "18446744073709551615") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "-" "min - max" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMin) - MpInt<8>(longLongMax);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "-18446744073709551615") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "*" "max * max" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMax) * MpInt<8>(longLongMax);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "85070591730234615847396907784232501249") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "*" "min * max" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMin) * MpInt<8>(longLongMax);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "-85070591730234615856620279821087277056") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "*" "max * min" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMax) * MpInt<8>(longLongMin);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "-85070591730234615856620279821087277056") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }

    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "*" "min * min" overflow test)") << std::endl;
    try {
        MpInt<8>(longLongMin) * MpInt<8>(longLongMin);
        std::cout << printWrong("Test failed") << std::endl;
        testFailed++;
    } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
        if (e.overflow.toDecimal() == "85070591730234615865843651857942052864") {
            std::cout << printRight("Test OK") << std::endl;
            testSuccess++;
        } else {
            std::cout << printWrong("Test failed") << std::endl;
            testFailed++;
        }
    }

    std::cout << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << printInfo("Overflow limits testing end") << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
}


void testRandomInts(std::size_t &success, std::size_t &failed) {
    std::random_device rd;
    std::size_t iterationSuccess, iterationFailed;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<long long int> random(intMin, intMax);
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << printInfo("Random int operations testing start") << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << std::endl;
    std::cout << printInfo("Operator \"+\"") << std::endl;

    iterationSuccess = iterationFailed = 0;
    for (int i = 0; i < 2000; i++) {
        auto a = random(eng);
        auto b = random(eng);
        auto aMpInt = MpInt<20>(a);
        auto bMpInt = MpInt<20>(b);
        auto c = aMpInt + bMpInt;
        if ((c == MpInt<20>(a + b))) {
            iterationSuccess++;
        } else {
            iterationFailed++;
        }
    }
    std::cout << printRight("Succeeded tests: ") << iterationSuccess << std::endl;
    std::cout << printWrong("Failed tests: ") << iterationFailed << std::endl;
    success += iterationSuccess;
    failed += iterationFailed;
    iterationSuccess = iterationFailed = 0;

    std::cout << std::endl;
    std::cout << printInfo("Operator \"-\"") << std::endl;
    for (int i = 0; i < 2000; i++) {
        auto a = random(eng);
        auto b = random(eng);
        auto aMpInt = MpInt<20>(a);
        auto bMpInt = MpInt<20>(b);
        auto c = aMpInt - bMpInt;
        if ((c == MpInt<20>(a - b))) {
            iterationSuccess++;
        } else {
            iterationFailed++;
        }
    }
    std::cout << printRight("Succeeded tests: ") << iterationSuccess << std::endl;
    std::cout << printWrong("Failed tests: ") << iterationFailed << std::endl;
    success += iterationSuccess;
    failed += iterationFailed;
    iterationSuccess = iterationFailed = 0;

    std::cout << std::endl;
    std::cout << printInfo("Operator \"*\"") << std::endl;

    std::random_device shortRd;
    std::mt19937_64 shortEng(shortRd());
    std::uniform_int_distribution<short> shortRandom(shortMin, shortMax);
    for (int i = 0; i < 2000; i++) {
        auto a = shortRandom(shortEng);
        auto b = shortRandom(shortEng);
        auto aMpInt = MpInt<20>(a);
        auto bMpInt = MpInt<20>(b);
        auto c = aMpInt * bMpInt;
        if ((c == MpInt<20>(static_cast<long long>(a) * b))) {
            iterationSuccess++;
        } else {
            iterationFailed++;
        }
    }
    std::cout << printRight("Succeeded tests: ") << iterationSuccess << std::endl;
    std::cout << printWrong("Failed tests: ") << iterationFailed << std::endl;
    success += iterationSuccess;
    failed += iterationFailed;
    iterationSuccess = iterationFailed = 0;


    std::cout << std::endl;
    std::cout << printInfo("Operator \"/\"") << std::endl;
    for (int i = 0; i < 2000; i++) {
        auto a = random(eng);
        auto b = random(eng);
        auto aMpInt = MpInt<20>(a);
        auto bMpInt = MpInt<20>(b);
        auto c = aMpInt / bMpInt;
        if ((c == MpInt<20>(a / b))) {
            iterationSuccess++;
        } else {
            iterationFailed++;
        }
    }
    std::cout << printRight("Succeeded tests: ") << iterationSuccess << std::endl;
    std::cout << printWrong("Failed tests: ") << iterationFailed << std::endl;
    success += iterationSuccess;
    failed += iterationFailed;

    std::cout << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << printInfo("Random int operations testing end") << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
}

void testFactorial(std::size_t &success, std::size_t &failed) {
    std::cout << std::endl;
    std::cout << printInfo("Factorial testing") << std::endl;
    auto a = MpInt<MP_INT_UNLIMITED>(23).factorial();
    if (a.toDecimal() == "25852016738884976640000") {
        success++;
        std::cout << printRight("Test OK") << std::endl;
    } else {
        failed++;
        std::cout << printWrong("Test failed") << std::endl;
    }
    auto b = MpInt<MP_INT_UNLIMITED>(100).factorial();
    if (b.toDecimal() ==
        "93326215443944152681699238856266700490715968264381621468592963895217599993229915608941463976156518286253697920827223758251185210916864000000000000000000000000") {
        success++;
        std::cout << printRight("Test OK") << std::endl;
    } else {
        failed++;
        std::cout << printWrong("Test failed") << std::endl;
    }
}

void test() {
    std::size_t testSuccess = 0;
    std::size_t testFailed = 0;

    testOverflow(testSuccess, testFailed);
    testRandomInts(testSuccess, testFailed);
    testFactorial(testSuccess, testFailed);

    std::cout << std::endl;
    std::cout << printInfo("Total tests: ") << (testSuccess + testFailed) << std::endl;
    std::cout << printRight("Succeeded tests: ") << (testSuccess) << std::endl;
    std::cout << printWrong("Failed tests: ") << (testFailed) << std::endl;
}

