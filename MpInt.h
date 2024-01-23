#pragma once

#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <algorithm>

constexpr std::size_t MP_INT_UNLIMITED = 0;
constexpr std::size_t MP_INT_MIN = 4;

template<std::size_t bytePrecision> concept SizeLimitation = (bytePrecision >= MP_INT_MIN ||
                                                              bytePrecision == MP_INT_UNLIMITED);
typedef std::int64_t bitsetItem;
constexpr std::size_t ELEMENT_BIT_SIZE = sizeof(bitsetItem) * 8;
constexpr bitsetItem EMPTY_ITEM = 0;
constexpr bitsetItem FULL_ITEM = -1;

template<class type>
class MpIntException : public std::exception {
public:
    const type overflow;

    explicit MpIntException(const type &of) : overflow(of) {
    }
};


template<std::size_t bytePrecision> requires SizeLimitation<bytePrecision>
class MpInt {
    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ CONSTEXPR -------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    static constexpr std::size_t bitPrecision = bytePrecision * 8;

    template<std::size_t bytePrecision1, std::size_t bytePrecision2>
    static constexpr std::size_t maxPrecision =
            bytePrecision1 == MP_INT_UNLIMITED || bytePrecision2 == MP_INT_UNLIMITED ? MP_INT_UNLIMITED : std::max(
                    bytePrecision1, bytePrecision2);

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ VARIABLES -------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
private:
    std::vector<bitsetItem> bitset;
    bool negative = false;

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ CONSTRUCTORS ----------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    MpInt() = default;

    /** Copy constructor */
    MpInt(const MpInt &other) : bitset(other.bitset), negative(other.negative) {}

    /** Copy assigment */
    MpInt &operator=(const MpInt &other) {
        this->bitset = other.bitset;
        this->negative = other.negative;
        return *this;
    }

    /** Move constructor */
    MpInt(MpInt &&other) noexcept: bitset(std::move(other.bitset)), negative(std::move(other.negative)) {}

    /** Move assigment */
    MpInt &operator=(MpInt &&other) noexcept {
        this->bitset = std::move(other.bitset);
        this->negative = std::move(other.negative);
        return *this;
    }

    /** Value assign */
    explicit MpInt(long long in) {
        this->negative = in < 0;
        std::bitset<sizeof(long long) * 8> bits(in);
        for (auto i = 0; i < sizeof(long long) * 8; i++) this->setBool(i, bits[i]);
    }

    /** Other size constructor */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    explicit MpInt(const MpInt<otherBytePrecision> &other) noexcept {
        this->negative = other.isNegative();
        for (auto i = 0; i < other.getCurrentCapacity(); i++) {
            this->setBool(i, other.getBool(i));
        }
    }

    /** Other size assign */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    MpInt &operator=(const MpInt<otherBytePrecision> &other) noexcept {
        this->negative = other.isNegative();
        for (auto i = 0; i < other.getCurrentCapacity(); i++) {
            this->setBool(i, other.getBool(i));
        }
        return *this;
    }

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ STATIC ---------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
private:


    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ METHODS ---------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    [[nodiscard]] bool isNegative() const {
        return this->negative;
    }

    void setNegative(bool value) {
        this->negative = value;
    }

    [[nodiscard]] std::size_t getCurrentCapacity() const {
        return this->bitset.size() * sizeof(bitsetItem) * 8;
    }

    [[nodiscard]] inline bool getBool(int position) const {
        if (position >= getCurrentCapacity()) {
            return this->isNegative();
        }
        std::size_t index = position / ELEMENT_BIT_SIZE;
        std::size_t offset = position % ELEMENT_BIT_SIZE;
        return (bitset[index] & (bitsetItem(1) << offset)) != 0;
    }

    inline void setBool(int position, bool value = true) {
        checkAndResize(position);
        std::size_t index = position / ELEMENT_BIT_SIZE;
        std::size_t offset = position % ELEMENT_BIT_SIZE;
        if (value) {
            bitset[index] |= bitsetItem(1) << offset;
        } else {
            bitset[index] &= ~(bitsetItem(1) << offset);
        }
    }

    [[nodiscard]] MpInt copy() const {
        MpInt result;
        for (int i = 0; i < this->getCurrentCapacity(); i++) {
            result.setBool(i, this->getBool(i));
        }
        result.negative = this->negative;
        return result;
    }

    [[nodiscard]] inline MpInt abs() const {
        auto copy = this->copy();
        if (copy.isNegative()) {
            copy.secondComplementReverse();
        }
        copy.negative = false;
        return copy;
    }

    void reset() {
        std::fill(bitset.begin(), bitset.end(), 0);
    }

    void secondComplement() {
        *this = ~(*this);
        *this = *this + MpInt<4>(1LL);
    }

    void secondComplementReverse() {
        try {
            *this = *this - MpInt<4>(1LL);
        } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
            *this = e.overflow;
        }
        *this = ~(*this);
    }

    [[nodiscard]] int getTopBit() const {
        for (int i = this->getCurrentCapacity(); i >= 0; i--) {
            if (this->getBool(i) != this->isNegative()) {
                return i;
            }
        }
        return -1;
    }

    MpInt factorial() {
        MpInt<MP_INT_UNLIMITED> result(1LL);
        MpInt<MP_INT_UNLIMITED> i(2LL);
        for (; i <= *this; i = i + MpInt<4>(1LL)) {
            result = result * i;
        }
        if (this->bitPrecision != MP_INT_UNLIMITED &&
            this->bitPrecision * 8 < result.getTopBit()) {
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(result);
        }
        return MpInt<bytePrecision>(result);
    }


private:
    void resize(std::size_t newSize) {
        for (auto i = this->getCurrentCapacity(); i < newSize; i++) {
            if ((i % ELEMENT_BIT_SIZE) == 0) {
                this->bitset.push_back(0);
            }
        }
    }

    void checkAndResize(const std::size_t pos) {
        if (pos >= getCurrentCapacity()) {
            resize(pos + 1);
        }
    }

    void expand(int fromIndex) {
        if (fromIndex % ELEMENT_BIT_SIZE == 0) {
            this->bitset.push_back(0);
        }
        for (int i = fromIndex; i < this->getCurrentCapacity(); i++) {
            this->setBool(i, this->isNegative());
        }
    }

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ OPERATORS -------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    MpInt operator~() {
        for (long &i: this->bitset) {
            i = ~i;
        }
        return *this;
    }

    MpInt &operator<<=(std::size_t shiftCount) {
        if (shiftCount >= getCurrentCapacity()) {
            reset();
            return *this;
        }
        if (shiftCount == 0) {
            return *this;
        }

        for (int i = this->getTopBit(); i >= 0; i--) {
            this->setBool(i + 1, this->getBool(i));
        }
        this->setBool(0, false);
        return *this;
    }

    MpInt &operator>>=(std::size_t shiftCount) {
        if (shiftCount >= getCurrentCapacity()) {
            reset();
            return *this;
        }
        if (shiftCount == 0) {
            return *this;
        }
        auto topBit = this->getTopBit();
        if (topBit == -1) {
            return *this;
        }
        for (int i = 0; i < topBit; i++) {
            this->setBool(i, this->getBool(i + 1));
        }
        this->setBool(topBit, false);
        return *this;
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator+(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        bool carry = false;
        std::size_t index = 0;
        auto maxCapacity = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        MpInt<maxPrecision<bytePrecision, otherBytePrecision>> result;
        for (index = 0; index < maxCapacity; index++) {
            bool b1 = a.getBool(index);
            bool b2 = b.getBool(index);
            result.setBool(index, b1 ^ b2 ^ carry);
            carry = (b1 && b2) || (b1 && carry) || (b2 && carry);
        }
        result.negative = result.getBool(index - 1);
        if ((!a.isNegative() && !b.isNegative() && result.isNegative()) ||
            (a.isNegative() && b.isNegative() && !result.isNegative())) {
            result.setNegative(!result.isNegative());
            result.expand(index);
            if (maxPrecision<bytePrecision, otherBytePrecision> != MP_INT_UNLIMITED) {
                throw MpIntException<MpInt<MP_INT_UNLIMITED>>(MpInt<MP_INT_UNLIMITED>(result));
            }
        }
        return result;
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator-(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        bool borrow = false, diff;
        int index;
        auto maxCapacity = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        MpInt<maxPrecision<bytePrecision, otherBytePrecision>> result;
        for (index = 0; index < maxCapacity; index++) {
            bool b1 = a.getBool(index);
            bool b2 = b.getBool(index);
            if (borrow) {
                diff = !(b1 ^ b2);
                borrow = !b1 || b2;
            } else {
                diff = b1 ^ b2;
                borrow = !b1 && b2;
            }
            result.setBool(index, diff);
        }
        result.negative = result.getBool(index - 1);
        if ((a.isNegative() && !b.isNegative() && !result.negative) ||
            (!a.isNegative() && b.isNegative() && result.negative)) {
            result.negative = !result.isNegative();
            result.expand(index);
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(MpInt<MP_INT_UNLIMITED>(result));
        }
        return result;
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator*(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const auto ZERO = MpInt<4>(0);
        auto aNegative = a.isNegative();
        auto bNegative = b.isNegative();
        auto aCopy = a.abs();
        auto bCopy = b.abs();
        MpInt<MP_INT_UNLIMITED> tmpResult;
        if (aCopy.getTopBit() >= bCopy.getTopBit()) {
            for (int i = 0; i <= aCopy.getTopBit(); i++) {
                if (aCopy.getBool(i)) {
                    tmpResult += bCopy;
                }
                bCopy <<= 1;
                if (bCopy.getBool(bCopy.getCurrentCapacity() - 1)) {
                    bCopy.expand(bCopy.getCurrentCapacity());
                }
            }
        } else {
            for (int i = 0; i <= bCopy.getTopBit(); i++) {
                if (bCopy.getBool(i)) {
                    tmpResult += aCopy;
                }
                aCopy <<= 1;
                if (aCopy.getBool(aCopy.getCurrentCapacity() - 1)) {
                    aCopy.expand(aCopy.getCurrentCapacity());
                }
            }
        }
        if (aNegative ^ bNegative) {
            tmpResult.setNegative(true);
            tmpResult.secondComplement();
        }
        if (maxPrecision<bytePrecision, otherBytePrecision> != MP_INT_UNLIMITED &&
            maxPrecision<bytePrecision, otherBytePrecision> * 8 < tmpResult.getTopBit()) {
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(tmpResult);
        }
        return MpInt<maxPrecision<bytePrecision, otherBytePrecision>>(tmpResult);
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator/(const MpInt<bytePrecision> &divident, const MpInt<otherBytePrecision> &divisor) {
        const MpInt<4> ONE = MpInt<4>(1LL);
        MpInt<std::max(bytePrecision, otherBytePrecision)> quotient;
        auto dividentNegative = divident.isNegative();
        auto divisorNegative = divisor.isNegative();
        auto dividentCopy = divident.abs();
        auto divisorCopy = divisor.abs();

        while (dividentCopy > divisorCopy) {
            quotient = quotient + ONE;
            dividentCopy = dividentCopy - divisorCopy;
        }
        if (dividentNegative ^ divisorNegative) {
            quotient.setNegative(true);
            quotient.secondComplement();
        }
        return quotient;
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator/=(const MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &divisor) {
        thiz = (thiz / divisor);
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator+=(MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &adder) {
        return (thiz = thiz + adder);
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator-=(MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &subtracter) {
        return (thiz = thiz - subtracter);
    }

    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator*=(MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &multiplier) {
        return (thiz = thiz * multiplier);
    }


    template<std::size_t otherPrecision>
    requires SizeLimitation<otherPrecision>
    bool operator==(MpInt<otherPrecision> &other) const {
        if (this->isNegative() != other.isNegative()) {
            return false;
        }
        for (auto i = 0; i < std::max(this->bitPrecision, other.bitPrecision); i++) {
            if (this->getBool(i) != other.getBool(i)) {
                return false;
            }
        }
        return true;
    }

    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator>=(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.bitPrecision, a.bitPrecision);
        if (a.isNegative() && !b.isNegative()) return false;
        if (!a.isNegative() && b.isNegative()) return true;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBool(index) && !b.getBool(index)) return !bothNegative;
            if (!a.getBool(index) && b.getBool(index)) return bothNegative;
        }
        return true;
    }

    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator>(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        if (a.isNegative() && !b.isNegative()) return false;
        if (!a.isNegative() && b.isNegative()) return true;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBool(index) && !b.getBool(index)) return !bothNegative;
            if (!a.getBool(index) && b.getBool(index)) return bothNegative;
        }
        return false;
    }

    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator<=(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        if (a.isNegative() && !b.isNegative()) return true;
        if (!a.isNegative() && b.isNegative()) return false;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBool(index) && !b.getBool(index)) return bothNegative;
            if (!a.getBool(index) && b.getBool(index)) return !bothNegative;
        }
        return true;
    }

    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator<(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        if (a.isNegative() && !b.isNegative()) return true;
        if (!a.isNegative() && b.isNegative()) return false;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBool(index) && !b.getBool(index)) return bothNegative;
            if (!a.getBool(index) && b.getBool(index)) return !bothNegative;
        }
        return false;
    }


    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ OUTPUT ----------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:

    [[nodiscard]] std::string toBinary() const {
        std::string res;
        if (getCurrentCapacity() == 0) {
            return "0";
        }
        for (auto i = 0; i < getCurrentCapacity(); i++) {
            res += this->getBool(i) ? '1' : '0';
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    [[nodiscard]] std::string toDecimal() const {
        auto copy = this->abs();
        auto binary = copy.toBinary();
        std::vector<int> intNumbers, tmpIntNumbers;
        int bitLength, tmpIntLength, intLength, index;

        intNumbers.resize(binary.length());
        tmpIntNumbers.resize(binary.length());
        tmpIntNumbers[0] = 1;
        tmpIntLength = 1;
        intLength = 1;
        bitLength = binary.length();
        for (int i = bitLength - 1; i >= 0; i--) {
            if (binary[i] == '1') {
                for (index = 0; index < tmpIntLength; index++) {
                    intNumbers[index] +=
                            tmpIntNumbers[index];
                    if (intNumbers[index] > 9) {
                        intNumbers[index + 1]++;
                        intNumbers[index] -= 10;
                    }
                }
                intLength = index;
                if (intNumbers[index] != 0) {
                    intLength++;
                }
            }
            for (index = 0; index < tmpIntLength; index++) {
                tmpIntNumbers[index] += tmpIntNumbers[index];
            }
            for (index = 0; index < tmpIntLength; index++) {
                if (tmpIntNumbers[index] > 9) {
                    tmpIntNumbers[index] -= 10;
                    tmpIntNumbers[index + 1]++;
                }
            }
            if (tmpIntNumbers[index] != 0) {
                tmpIntLength++;
            }
        }
        std::string result;
        for (int i = intLength - 1; i >= 0; i--) {
            result += std::to_string(intNumbers[i]);
        }
        return this->negative ? '-' + result : result;
    }
};

