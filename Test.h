
#include <iostream>
#include <limits>
#include <random>
#include "MpInt.h"

#define COLORED

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

void testOverflow(std::size_t &testSuccess, std::size_t &testFailed) {
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << printInfo("Overflow limits testing start") << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    auto min = std::numeric_limits<long long>::min();
    auto max = std::numeric_limits<long long>::max();
    std::cout << std::endl;
    std::cout << printInfo(R"(Operator "+" "max + max" overflow test)") << std::endl;
    try {
        auto res = MpInt<8>(max) + MpInt<8>(max);
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
        MpInt<8>(min) + MpInt<8>(min);
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
        MpInt<8>(max) - MpInt<8>(min);
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
        MpInt<8>(min) - MpInt<8>(max);
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
        MpInt<8>(max) * MpInt<8>(max);
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
        MpInt<8>(min) * MpInt<8>(max);
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
        MpInt<8>(max) * MpInt<8>(min);
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
        MpInt<8>(min) * MpInt<8>(min);
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
    auto min = std::numeric_limits<int>::min();
    auto max = std::numeric_limits<int>::max();
    std::random_device rd;
    std::size_t iterationSuccess, iterationFailed;
    std::mt19937_64 eng(rd());
    std::uniform_int_distribution<long long int> random(min, max);
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
    auto shortMin = std::numeric_limits<short>::min();
    auto shortMax = std::numeric_limits<short>::max();
    std::random_device shortRd;
    std::mt19937_64 shortEng(shortRd());
    std::uniform_int_distribution<short> shortRandom(shortMin, shortMax);
    for (int i = 0; i < 2000; i++) {
        auto a = shortRandom(shortEng);
        auto b = shortRandom(shortEng);
        auto aMpInt = MpInt<20>(a);
        auto bMpInt = MpInt<20>(b);
        auto c = aMpInt * bMpInt;
        if ((c == MpInt<20>(a * b))) {
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
    auto b = MpInt<MP_INT_UNLIMITED>(458).factorial();
    if (b.toDecimal() == "31558612972732848195962088165060452962725279687046663105393999903323304169952611682865651645393421280420722606506218993066954719646651702892677775173715800913698684882354527997435769239386955037843076474999638393632721854444452389067259702907451597235149877727954079313542244508898054659724763317027896391801647990997688965012299141147629169118289352079290183685864157414436690916091756636546184601926428526040370371705924260743821590161502585889740284513056760550850222960023407067493552163256040177559330634900229807062247776118040898052904722356431819321044396447152215178722499399094446240445973231693591592528512671137419540166276433460035447275152173812845055162242773108961668084977384924332233161709385419710116813642746963289736181420676108553242062712849431714137421183675834118168270988564418928710515808529877566908903143921770591626873816869293486412467892599987520375781119996158147849538303426560000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000") {
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

