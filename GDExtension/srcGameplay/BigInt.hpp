#ifndef BigInt_hpp
#define BigInt_hpp

#include <cstdint>
#include <string>
#include <vector>

class BigInt
{
private:
    std::vector<uint32_t> digits;
    bool isNegative;

public:
    static constexpr uint8_t VAL_EQUALS = 0;
    static constexpr uint8_t VAL_LESS_THAN = 1;
    static constexpr uint8_t VAL_GREATER_THAN = 2;
    static constexpr size_t SHIFT_DIGIT_BITS = sizeof(uint32_t) * 8;
    static constexpr size_t SHIFT_ZERO_BITS = 0;

    void assignFromUInt64(const uint64_t value);
    void assignFromInt64(const int64_t value);
    void assignFromUInt32(const uint32_t value);
    void assignFromInt32(const int32_t value);
    void assignFromString(const std::string value, bool& success);
    void assignZero();

    uint64_t toUInt64(bool& success);
    int64_t toInt64(bool& success);
    uint32_t toUInt32(bool& success);
    int32_t toInt32(bool& success);
    std::string toString();

    static void add(const BigInt& a, const BigInt& b, BigInt& result);
    static void subtract(const BigInt& a, const BigInt& b, BigInt& result);
    static void multiply(const BigInt& a, const BigInt& b, BigInt& result);
    static void divide(const BigInt& a, const BigInt& b, BigInt& result);
    static void modulus(const BigInt& a, const BigInt& b, BigInt& result);
    static void GCD(const BigInt& a, const BigInt& b, BigInt& result);

    void shiftLeft(size_t bits);
    void shiftRight(size_t bits);

    void absoluteValue();
    void negate();

    uint8_t compareTo(const BigInt& other) const;
    bool equals(const BigInt& other) const;
    bool lessThan(const BigInt& other) const;
    bool greaterThan(const BigInt& other) const;
    bool notEquals(const BigInt& other) const;
    bool lessThanEqualsTo(const BigInt& other) const;
    bool greaterThanEqualsTo(const BigInt& other) const;

    bool isEqualToZero();
    bool isEqualToPositiveOne();
    bool isEqualToNegativeOne();
    bool getIsNegative();

private:
    void helperNormalize();
    void helperAddUnsigned(const BigInt& other);
    void helperAddShiftedUInt64Unsigned(uint64_t value, size_t shiftAmmount);
    void helperSubtractUnsigned(const BigInt& other);
    void helperSubtractReversedUnsigned(const BigInt& other);
    void helperMultiplyUnsigned(const BigInt& other);
    void helperDivideUnsigned(const BigInt& other);
    uint8_t helperCompareToUnsigned(const BigInt& other) const;
    bool helperEqualsUnsigned(const BigInt& other) const;
    bool helperLessThanUnsigned(const BigInt& other) const;
    bool helperGreaterThanUnsigned(const BigInt& other) const;
    bool helperNotEqualsUnsigned(const BigInt& other) const;
    bool helperLessThanEqualsToUnsigned(const BigInt& other) const;
    bool helperGreaterThanEqualsToUnsigned(const BigInt& other) const;
};

class BigIntConst
{
private:
    static BigInt createOne();
    static BigInt createTen();

public:
    static const BigInt one;
    static const BigInt ten;
};

#endif