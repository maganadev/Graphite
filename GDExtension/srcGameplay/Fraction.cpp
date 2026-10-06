#include "Fraction.hpp"

#include <string>

Fraction::Fraction()
{
    // Initialize to 0/1
    assignZero();
}

Fraction::~Fraction()
{
    // Empty destructor
}

void Fraction::assignFromBigInt(const BigInt& other)
{
    numerator = other;
    denominator.assignFromUInt32(1);
    helperNormalize();
}

void Fraction::assignFromUInt64(const uint64_t value)
{
    numerator.assignFromUInt64(value);
    denominator.assignFromUInt32(1);
    helperNormalize();
}

void Fraction::assignFromInt(const int64_t value)
{
    numerator.assignFromInt64(value);
    denominator.assignFromUInt32(1);
    helperNormalize();
}

void Fraction::assignFromString(const std::string value, bool& success)
{
    size_t slashPos = value.find('/');
    size_t periodPos = value.find('.');

    bool slashFound = slashPos != std::string::npos;
    bool periodFound = periodPos != std::string::npos;

    if (slashFound && !periodFound)
    {
        std::string numeratorStr = value.substr(0, slashPos);
        std::string denominatorStr = value.substr(slashPos + 1);

        numerator.assignFromString(numeratorStr, success);
        if (!success)
        {
            success = false;
            return;
        }

        denominator.assignFromString(denominatorStr, success);
        if (!success)
        {
            success = false;
            return;
        }

        // A zero denominator would make every later multiply/divide undefined.
        if (denominator.isEqualToZero())
        {
            success = false;
            return;
        }

        helperNormalize();
        success = true;
        return;
    }
    else if (!slashFound && periodFound)
    {
        bool shouldNegateEndResult = false;

        std::string integerPartStr = value.substr(0, periodPos);
        std::string fractionalPartStr = value.substr(periodPos + 1);

        BigInt integerPart;
        integerPart.assignFromString(integerPartStr, success);
        if (!success)
        {
            success = false;
            return;
        }

        bool wasResultNegative = integerPart.getIsNegative();
        if (wasResultNegative)
        {
            integerPart.negate();
            shouldNegateEndResult = !shouldNegateEndResult;
        }

        BigInt fractionalPart;
        fractionalPart.assignFromString(fractionalPartStr, success);
        if (!success)
        {
            success = false;
            return;
        }

        wasResultNegative = fractionalPart.getIsNegative();
        if (wasResultNegative)
        {
            fractionalPart.negate();
            shouldNegateEndResult = !shouldNegateEndResult;
        }

        denominator.assignFromUInt32(1);
        for (size_t i = 0; i < fractionalPartStr.length(); ++i)
        {
            BigInt::multiply(denominator, BigIntConst::ten, denominator);
        }

        BigInt::multiply(integerPart, denominator, integerPart);
        BigInt::add(integerPart, fractionalPart, integerPart);

        numerator = integerPart;

        if (shouldNegateEndResult)
        {
            numerator.negate();
        }

        helperNormalize();
        success = true;
        return;
    }
    else if (!slashFound && !periodFound)
    {
        numerator.assignFromString(value, success);

        if (!success)
        {
            success = false;
            return;
        }

        denominator.assignFromUInt32(1);

        helperNormalize();
        success = true;
        return;
    }
    else
    {
        success = false;
        return;
    }
}

void Fraction::assignZero()
{
    // Guaranteed already normalized (0/1 is in lowest terms)
    numerator.assignZero();
    denominator.assignFromUInt32(1);
}

BigInt Fraction::toBigInt()
{
    BigInt toReturn;
    toReturn = numerator;
    BigInt::divide(toReturn, denominator, toReturn);
    return toReturn;
}

uint64_t Fraction::toUInt(bool& success)
{
    uint64_t toReturn = 0;
    BigInt forCalculations = numerator;
    BigInt::divide(forCalculations, denominator, forCalculations);
    toReturn = forCalculations.toUInt64(success);
    return toReturn;
}

int64_t Fraction::toInt(bool& success)
{
    int64_t toReturn = 0;
    BigInt forCalculations = numerator;
    BigInt::divide(forCalculations, denominator, forCalculations);
    toReturn = forCalculations.toInt64(success);
    return toReturn;
}

double Fraction::toDouble(bool& success)
{
    if (denominator.isEqualToZero())
    {
        success = false;
        return 0.0;
    }

    std::string numeratorStr = numerator.toString();
    std::string denominatorStr = denominator.toString();

    double numeratorDouble = std::stod(numeratorStr);
    double denominatorDouble = std::stod(denominatorStr);

    success = true;
    return numeratorDouble / denominatorDouble;
}

std::string Fraction::toDecimalString(size_t decimalPlaces) const
{
    std::string toReturn;
    BigInt temp = numerator;

    if (decimalPlaces != 0)
    {
        for (size_t i = 0; i < decimalPlaces; i++)
        {
            BigInt::multiply(temp, BigIntConst::ten, temp);
        }
    }

    BigInt::divide(temp, denominator, temp);

    toReturn = temp.toString();

    if (decimalPlaces != 0)
    {
        if (decimalPlaces <= toReturn.length())
        {
            toReturn.insert(toReturn.length() - decimalPlaces, 1, '.');
        }
    }

    return toReturn;
}

std::string Fraction::toFractionString() const
{
    BigInt tempNum = numerator;
    BigInt tempDen = denominator;

    if (tempDen.getIsNegative())
    {
        tempNum.negate();
        tempDen.negate();
    }

    return tempNum.toString() + "/" + tempDen.toString();
}

void Fraction::add(const Fraction& other)
{
    BigInt newGCD;
    BigInt::GCD(this->denominator, other.denominator, newGCD);

    BigInt targetDen;
    BigInt::multiply(this->denominator, other.denominator, targetDen);
    BigInt::divide(targetDen, newGCD, targetDen);

    BigInt temp;
    BigInt::divide(targetDen, this->denominator, temp);
    BigInt ANewNum;
    BigInt::multiply(this->numerator, temp, ANewNum);

    BigInt::divide(targetDen, other.denominator, temp);
    BigInt BNewNum;
    BigInt::multiply(other.numerator, temp, BNewNum);

    BigInt targetNum;
    BigInt::add(ANewNum, BNewNum, targetNum);

    numerator = targetNum;
    denominator = targetDen;

    helperNormalize();
}

void Fraction::subtract(const Fraction& other)
{
    BigInt newGCD;
    BigInt::GCD(this->denominator, other.denominator, newGCD);

    BigInt targetDen;
    BigInt::multiply(this->denominator, other.denominator, targetDen);
    BigInt::divide(targetDen, newGCD, targetDen);

    BigInt temp;
    BigInt::divide(targetDen, this->denominator, temp);
    BigInt ANewNum;
    BigInt::multiply(this->numerator, temp, ANewNum);

    BigInt::divide(targetDen, other.denominator, temp);
    BigInt BNewNum;
    BigInt::multiply(other.numerator, temp, BNewNum);

    BigInt targetNum;
    BigInt::subtract(ANewNum, BNewNum, targetNum);

    numerator = targetNum;
    denominator = targetDen;

    helperNormalize();
}

void Fraction::multiply(const Fraction& other)
{
    BigInt::multiply(numerator, other.numerator, numerator);
    BigInt::multiply(denominator, other.denominator, denominator);

    helperNormalize();
}

void Fraction::divide(const Fraction& other)
{
    BigInt::multiply(numerator, other.denominator, numerator);
    BigInt::multiply(denominator, other.numerator, denominator);

    helperNormalize();
}

void Fraction::absoluteValue()
{
    numerator.absoluteValue();

    helperNormalize();
}

void Fraction::negate()
{
    numerator.negate();

    helperNormalize();
}

uint8_t Fraction::compareTo(const Fraction& other) const
{
    BigInt leftSide;
    BigInt rightSide;
    BigInt::multiply(this->numerator, other.denominator, leftSide);
    BigInt::multiply(this->denominator, other.numerator, rightSide);
    return leftSide.compareTo(rightSide);
}

bool Fraction::equals(const Fraction& other) const
{
    return (compareTo(other) == VAL_EQUALS);
}

bool Fraction::lessThan(const Fraction& other) const
{
    return (compareTo(other) == VAL_LESS_THAN);
}

bool Fraction::greaterThan(const Fraction& other) const
{
    return (compareTo(other) == VAL_GREATER_THAN);
}

bool Fraction::notEquals(const Fraction& other) const
{
    return (compareTo(other) != VAL_EQUALS);
}

bool Fraction::lessThanEqualsTo(const Fraction& other) const
{
    return (compareTo(other) != VAL_GREATER_THAN);
}

bool Fraction::greaterThanEqualsTo(const Fraction& other) const
{
    return (compareTo(other) != VAL_LESS_THAN);
}

void Fraction::helperNormalize()
{
    BigInt newGCD;
    BigInt::GCD(numerator, denominator, newGCD);

    if (newGCD.isEqualToPositiveOne())
    {
        return;
    }

    BigInt::divide(numerator, newGCD, numerator);
    BigInt::divide(denominator, newGCD, denominator);
}