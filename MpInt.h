#pragma once

#include <iostream>
#include <bitset>
#include <cmath>
#include <vector>
#include <algorithm>

/** Template argument for unlimited number precision */
constexpr std::size_t MP_INT_UNLIMITED = 0;
/** Template argument for minimal number precision */
constexpr std::size_t MP_INT_MIN = 4;

/** Concept for number precision limitation */
template<std::size_t bytePrecision> concept SizeLimitation = (bytePrecision >= MP_INT_MIN ||
                                                              bytePrecision == MP_INT_UNLIMITED);
/** Item struct of binary system inside MpInt */
typedef std::int64_t bitsetItem;
/** Bit size of one element of bitset */
constexpr std::size_t ELEMENT_BIT_SIZE = sizeof(bitsetItem) * 8;

/**
 * @brief Exception thrown from inside of MpInt arithmetic operations. It is thrown usually on overflow.
 * @tparam type Type to be held.
 */
template<class type>
class MpIntException : public std::exception {
public:
    const type overflow;

    explicit MpIntException(const type &of) : overflow(of) {
    }
};


/**
 * @brief Class representing dynamic precision number.
 * @tparam bytePrecision Maximal number precision in bytes. Zero if unlimited.
 */
template<std::size_t bytePrecision> requires SizeLimitation<bytePrecision>
class MpInt {
    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ CONSTEXPR -------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    /** Precision of number in bytes. Zero if unlimited */
    static constexpr std::size_t bitPrecision = bytePrecision * 8;

private:
    /** Computation of max precision. Important because 0>N. */
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
    /** Vector representing bits of number */
    std::vector<bitsetItem> bitset;
    /** Bool representing number positivity/negativity. */
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
        for (auto i = 0; i < sizeof(long long) * 8; i++) this->setBit(i, bits[i]);
    }

    /** Other size constructor */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    explicit MpInt(const MpInt<otherBytePrecision> &other) noexcept {
        this->negative = other.isNegative();
        for (auto i = 0; i < other.getCurrentCapacity(); i++) {
            this->setBit(i, other.getBit(i));
        }
    }

    /** Other size assign */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    MpInt &operator=(const MpInt<otherBytePrecision> &other) noexcept {
        this->negative = other.isNegative();
        for (auto i = 0; i < other.getCurrentCapacity(); i++) {
            this->setBit(i, other.getBit(i));
        }
        return *this;
    }

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ METHODS ---------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    /**
     * @brief Set negative flag.
     */
    void setNegative(bool value) {
        this->negative = value;
    }

    /**
     * @return Negativity flag.
     */
    [[nodiscard]] bool isNegative() const {
        return this->negative;
    }

    /**
     * @return Current capacity of bitset in bits. Can be higher than max bits (padding).
     */
    [[nodiscard]] std::size_t getCurrentCapacity() const {
        return this->bitset.size() * sizeof(bitsetItem) * 8;
    }

    /**
     * @param position Position of bit.
     * @return Access bit on position. If position is above capacity, negativity flag is returned.
     */
    [[nodiscard]] inline bool getBit(int position) const {
        if (position >= getCurrentCapacity()) {
            return this->isNegative();
        }
        std::size_t index = position / ELEMENT_BIT_SIZE;
        std::size_t offset = position % ELEMENT_BIT_SIZE;
        return (bitset[index] & (bitsetItem(1) << offset)) != 0;
    }

    /**
     * @brief Set bit on position to value. If position is greater than maximal capacity, expand bitset.
     * @param position Position of bit.
     * @param value Value of bit.
     */
    inline void setBit(int position, bool value = true) {
        checkAndResize(position);
        std::size_t index = position / ELEMENT_BIT_SIZE;
        std::size_t offset = position % ELEMENT_BIT_SIZE;
        if (value) {
            bitset[index] |= bitsetItem(1) << offset;
        } else {
            bitset[index] &= ~(bitsetItem(1) << offset);
        }
    }

    /**
     * @return Copy of this.
     */
    [[nodiscard]] MpInt copy() const {
        MpInt result;
        for (int i = 0; i < this->getCurrentCapacity(); i++) {
            result.setBit(i, this->getBit(i));
        }
        result.negative = this->negative;
        return result;
    }

    /**
     * @return Absolute value of this.
     */
    [[nodiscard]] inline MpInt abs() const {
        auto copy = this->copy();
        if (copy.isNegative()) {
            copy.secondComplementReverse();
        }
        copy.negative = false;
        return copy;
    }

    /**
     * @brief Reset number to 0.
     */
    void reset() {
        this->bitset = std::vector<bitsetItem>();
        this->setNegative(false);
    }

    /**
     * @brief Make second complement of current number. May be used for reversing from positive to negative.
     */
    void secondComplement() {
        *this = ~(*this);
        *this = *this + MpInt<4>(1LL);
    }

    /**
 * @brief Make reversed second complement of current number. May be used for reversing from negative to positive.
 */
    void secondComplementReverse() {
        try {
            *this = *this - MpInt<4>(1LL);
        } catch (MpIntException<MpInt<MP_INT_UNLIMITED>> &e) {
            *this = e.overflow;
        }
        *this = ~(*this);
    }

    /**
     * @return Index of most significant bit of current number or -1 if nothing was found.
     */
    [[nodiscard]] int getTopBit() const {
        for (int i = this->getCurrentCapacity(); i >= 0; i--) {
            if (this->getBit(i) != this->isNegative()) {
                return i;
            }
        }
        return -1;
    }

    /**
     * @brief Compute factorial from this and return computed copied number.
     * @return Computed number.
     */
    [[nodiscard]] MpInt<bytePrecision> factorial() const {
        MpInt<MP_INT_UNLIMITED> result(1LL);
        MpInt<MP_INT_UNLIMITED> i(2LL);
        for (; i <= *this; i = i + MpInt<4>(1LL)) {
            result = result * i;
        }
        if (this->bitPrecision != MP_INT_UNLIMITED &&
            static_cast<long long>(bitPrecision) < result.getTopBit()) {
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(result);
        }
        return MpInt<bytePrecision>(result);
    }


private:
    /**
     * @brief Resize bitset to new size.
     */
    void resize(std::size_t newSize) {
        for (auto i = this->getCurrentCapacity(); i < newSize; i++) {
            if ((i % ELEMENT_BIT_SIZE) == 0) {
                this->bitset.push_back(0);
            }
        }
    }

    /**
     * @brief Check if bitset needs to be enlarged. If necessary, it enlarge bitset.
     * @param pos Position to check.
     */
    void checkAndResize(const std::size_t pos) {
        if (pos >= getCurrentCapacity()) {
            resize(pos + 1);
        }
    }

    /**
     * @brief Expand with negative flag values from index to max capacity.
     */
    void expand(int fromIndex) {
        if (fromIndex % ELEMENT_BIT_SIZE == 0) {
            this->bitset.push_back(0);
        }
        for (int i = fromIndex; i < this->getCurrentCapacity(); i++) {
            this->setBit(i, this->isNegative());
        }
    }

    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ OPERATORS -------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:
    /**
     * @brief Reverse all bits in bitset.
     * @return This with reversed bits.
     */
    MpInt operator~() {
        for (long &i: this->bitset) {
            i = ~i;
        }
        return *this;
    }

    /**
     * @brief Left shift of bitset.
     * @param shiftCount Number to be shifted to the left.
     * @return Shifted this.
     */
    MpInt &operator<<=(std::size_t shiftCount) {
        if (shiftCount >= getCurrentCapacity()) {
            reset();
            return *this;
        }
        if (shiftCount == 0) {
            return *this;
        }

        for (int i = this->getTopBit(); i >= 0; i--) {
            this->setBit(i + 1, this->getBit(i));
        }
        this->setBit(0, false);
        return *this;
    }

    /**
     * @brief Right shift of bitset.
     * @param shiftCount Number to be shifted to the right.
     * @return Shifted this.
    */
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
            this->setBit(i, this->getBit(i + 1));
        }
        this->setBit(topBit, false);
        return *this;
    }

    /**
     * @brief Add two numbers and return result addition. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return Sum of a and b (a + b).
     */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator+(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        bool carry = false;
        std::size_t index = 0;
        auto maxCapacity = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        MpInt<maxPrecision<bytePrecision, otherBytePrecision>> result;
        for (index = 0; index < maxCapacity; index++) {
            bool b1 = a.getBit(index);
            bool b2 = b.getBit(index);
            result.setBit(index, b1 ^ b2 ^ carry);
            carry = (b1 && b2) || (b1 && carry) || (b2 && carry);
        }
        result.negative = result.getBit(index - 1);
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

    /**
     * @brief Subtract two numbers and return result subtraction. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return Subtraction of b from a (a - b).
     */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator-(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        bool borrow = false, diff;
        int index;
        auto maxCapacity = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        MpInt<maxPrecision<bytePrecision, otherBytePrecision>> result;
        for (index = 0; index < maxCapacity; index++) {
            bool b1 = a.getBit(index);
            bool b2 = b.getBit(index);
            if (borrow) {
                diff = !(b1 ^ b2);
                borrow = !b1 || b2;
            } else {
                diff = b1 ^ b2;
                borrow = !b1 && b2;
            }
            result.setBit(index, diff);
        }
        result.negative = result.getBit(index - 1);
        if ((a.isNegative() && !b.isNegative() && !result.negative) ||
            (!a.isNegative() && b.isNegative() && result.negative)) {
            result.negative = !result.isNegative();
            result.expand(index);
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(MpInt<MP_INT_UNLIMITED>(result));
        }
        return result;
    }

    /**
     * @brief Multiply two numbers and return result. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return Multiplication of a and b (a * b).
     */
    template<std::size_t otherBytePrecision>
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
                if (aCopy.getBit(i)) {
                    tmpResult += bCopy;
                }
                bCopy <<= 1;
                if (bCopy.getBit(bCopy.getCurrentCapacity() - 1)) {
                    bCopy.expand(bCopy.getCurrentCapacity());
                }
            }
        } else {
            for (int i = 0; i <= bCopy.getTopBit(); i++) {
                if (bCopy.getBit(i)) {
                    tmpResult += aCopy;
                }
                aCopy <<= 1;
                if (aCopy.getBit(aCopy.getCurrentCapacity() - 1)) {
                    aCopy.expand(aCopy.getCurrentCapacity());
                }
            }
        }
        if (aNegative ^ bNegative) {
            tmpResult.setNegative(true);
            tmpResult.secondComplement();
        }
        if (maxPrecision<bytePrecision, otherBytePrecision> != MP_INT_UNLIMITED &&
            static_cast<long long>(maxPrecision<bytePrecision, otherBytePrecision>) * 8 < tmpResult.getTopBit()) {
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(tmpResult);
        }
        return MpInt<maxPrecision<bytePrecision, otherBytePrecision>>(tmpResult);
    }

    /**
     * @brief Divide two numbers and return result. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return Division of a and b (a / b).
    */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator/(const MpInt<bytePrecision> &divident, const MpInt<otherBytePrecision> &divisor) {
        const MpInt<4> ONE = MpInt<4>(1LL);
        const MpInt<4> ZERO = MpInt<4>(0LL);
        MpInt<std::max(bytePrecision, otherBytePrecision)> quotient;
        auto dividentNegative = divident.isNegative();
        auto divisorNegative = divisor.isNegative();
        auto dividentCopy = divident.abs();
        auto divisorCopy = divisor.abs();

        if (divisor == ZERO) {
            throw MpIntException<MpInt<MP_INT_UNLIMITED>>(MpInt<MP_INT_UNLIMITED>(0LL));
        }
        while (dividentCopy >= divisorCopy) {
            quotient = quotient + ONE;
            dividentCopy = dividentCopy - divisorCopy;
        }
        if (dividentNegative ^ divisorNegative) {
            quotient.setNegative(true);
            quotient.secondComplement();
        }
        return quotient;
    }

    /**
     * @brief Divide two numbers and return result. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param thiz Assign result to thiz.
     * @param divisor Divisor.
     * @return Division of a and b (a / b).
    */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator/=(const MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &divisor) {
        thiz = (thiz / divisor);
    }

    /**
     * @brief Add two numbers and return result. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param thiz Assign result to thiz.
     * @param adder Adder.
     * @return Addition of a and b (a + b).
    */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator+=(MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &adder) {
        return (thiz = thiz + adder);
    }

    /**
     * @brief Subtract two numbers and return result. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param thiz Assign result to thiz.
     * @param subtracter Subtracter.
     * @return Subtraction of a and b (a - b).
    */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator-=(MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &subtracter) {
        return (thiz = thiz - subtracter);
    }

    /**
     * @brief Multiplication two numbers and return result. Throw MpIntException if number limitation is overflowed.
     * @tparam otherBytePrecision Template of second parameter.
     * @param thiz Assign result to thiz.
     * @param subtracter Subtracter.
     * @return Multiplication of a and b (a * b).
    */
    template<size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend MpInt<maxPrecision<bytePrecision, otherBytePrecision>>
    operator*=(MpInt<bytePrecision> &thiz, const MpInt<otherBytePrecision> &multiplier) {
        return (thiz = thiz * multiplier);
    }

    /**
     * @brief Equals operator.
     * @tparam otherPrecision Template of second parameter.
     * @param other Second parameter.
     * @return True if numbers are equal, false otherwise.
     */
    template<std::size_t otherPrecision>
    requires SizeLimitation<otherPrecision>
    bool operator==(const MpInt<otherPrecision> &other) const {
        if (this->isNegative() != other.isNegative()) {
            return false;
        }
        for (auto i = 0; i < std::max(this->bitPrecision, other.bitPrecision); i++) {
            if (this->getBit(i) != other.getBit(i)) {
                return false;
            }
        }
        return true;
    }

    /**
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return True if a >= b. False otherwise.
     */
    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator>=(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.bitPrecision, a.bitPrecision);
        if (a.isNegative() && !b.isNegative()) return false;
        if (!a.isNegative() && b.isNegative()) return true;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBit(index) && !b.getBit(index)) return !bothNegative;
            if (!a.getBit(index) && b.getBit(index)) return bothNegative;
        }
        return true;
    }

    /**
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return True if a > b. False otherwise.
     */
    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator>(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        if (a.isNegative() && !b.isNegative()) return false;
        if (!a.isNegative() && b.isNegative()) return true;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBit(index) && !b.getBit(index)) return !bothNegative;
            if (!a.getBit(index) && b.getBit(index)) return bothNegative;
        }
        return false;
    }

    /**
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return True if a <= b. False otherwise.
     */
    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator<=(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        if (a.isNegative() && !b.isNegative()) return true;
        if (!a.isNegative() && b.isNegative()) return false;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBit(index) && !b.getBit(index)) return bothNegative;
            if (!a.getBit(index) && b.getBit(index)) return !bothNegative;
        }
        return true;
    }

    /**
     * @tparam otherBytePrecision Template of second parameter.
     * @param a First term.
     * @param b Second term.
     * @return True if a < b. False otherwise.
     */
    template<std::size_t otherBytePrecision>
    requires SizeLimitation<otherBytePrecision>
    friend bool operator<(const MpInt<bytePrecision> &a, const MpInt<otherBytePrecision> &b) {
        const size_t size = std::max(a.getCurrentCapacity(), b.getCurrentCapacity());
        if (a.isNegative() && !b.isNegative()) return true;
        if (!a.isNegative() && b.isNegative()) return false;
        bool bothNegative = a.isNegative();
        for (int index = size - 1; index >= 0; index--) {
            if (a.getBit(index) && !b.getBit(index)) return bothNegative;
            if (!a.getBit(index) && b.getBit(index)) return !bothNegative;
        }
        return false;
    }


    // ------------------------------------------------------
    // ------------------------------------------------------
    // ------------------ OUTPUT ----------------------------
    // ------------------------------------------------------
    // ------------------------------------------------------
public:

    /**
     * @return String of binary represented numbers.
     */
    [[nodiscard]] std::string toBinary() const {
        std::string res;
        if (getCurrentCapacity() == 0) {
            return "0";
        }
        for (auto i = 0; i < getCurrentCapacity(); i++) {
            res += this->getBit(i) ? '1' : '0';
        }
        std::reverse(res.begin(), res.end());
        return res;
    }

    /**
     * @brief Make decimal string from binary representation of number.
     * @return Decimal string.
     */
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

