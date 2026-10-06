#ifndef Fraction_hpp
#define Fraction_hpp

#include "BigInt.hpp"
#include <string>

class Fraction
{
private:
    BigInt numerator;
    BigInt denominator;

public:
    static constexpr uint8_t VAL_EQUALS = 0;
    static constexpr uint8_t VAL_LESS_THAN = 1;
    static constexpr uint8_t VAL_GREATER_THAN = 2;

    Fraction();
    ~Fraction();

    void assignFromBigInt(const BigInt& other);
    void assignFromUInt64(const uint64_t value);
    void assignFromInt(const int64_t value);
    void assignFromString(const std::string value, bool& success);
    void assignZero();

    BigInt toBigInt();
    uint64_t toUInt(bool& success);
    int64_t toInt(bool& success);
    double toDouble(bool& success);
    std::string toDecimalString(size_t decimalPlaces) const;
    std::string toFractionString() const;

    void add(const Fraction& other);
    void subtract(const Fraction& other);
    void multiply(const Fraction& other);
    void divide(const Fraction& other);
    void absoluteValue();
    void negate();

    uint8_t compareTo(const Fraction& other) const;
    bool equals(const Fraction& other) const;
    bool lessThan(const Fraction& other) const;
    bool greaterThan(const Fraction& other) const;
    bool notEquals(const Fraction& other) const;
    bool lessThanEqualsTo(const Fraction& other) const;
    bool greaterThanEqualsTo(const Fraction& other) const;

private:
    void helperNormalize();
};

#endif