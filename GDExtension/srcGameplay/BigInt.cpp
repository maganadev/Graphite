#include "BigInt.hpp"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

void BigInt::assignFromUInt64(const uint64_t value)
{
    uint32_t upperHalf = static_cast<uint32_t>((value >> SHIFT_DIGIT_BITS) & UINT32_MAX);
    uint32_t lowerHalf = static_cast<uint32_t>((value >> SHIFT_ZERO_BITS) & UINT32_MAX);

    isNegative = false;
    digits.resize(2);
    digits[0] = lowerHalf;
    digits[1] = upperHalf;

    helperNormalize();
}

void BigInt::assignFromInt64(const int64_t value)
{
    uint64_t newValue = 0;
    bool shouldBeNegative = false;
    if (value < 0)
    {
        shouldBeNegative = true;
        newValue = (value == INT64_MIN) ? static_cast<uint64_t>(INT64_MIN) : static_cast<uint64_t>(-value);
    }
    else
    {
        shouldBeNegative = false;
        newValue = static_cast<uint64_t>(value);
    }

    uint32_t upperHalf = static_cast<uint32_t>((newValue >> SHIFT_DIGIT_BITS) & UINT32_MAX);
    uint32_t lowerHalf = static_cast<uint32_t>((newValue >> SHIFT_ZERO_BITS) & UINT32_MAX);

    isNegative = shouldBeNegative;
    digits.resize(2);
    digits[0] = lowerHalf;
    digits[1] = upperHalf;

    helperNormalize();
}

void BigInt::assignFromUInt32(const uint32_t value)
{
    isNegative = false;
    digits.resize(1);
    digits[0] = value;

    helperNormalize();
}

void BigInt::assignFromInt32(const int32_t value)
{
    uint32_t newValue = 0;
    bool shouldBeNegative = false;
    if (value < 0)
    {
        shouldBeNegative = true;
        newValue = (value == INT32_MIN) ? static_cast<uint32_t>(INT32_MIN) : static_cast<uint32_t>(-value);
    }
    else
    {
        shouldBeNegative = false;
        newValue = static_cast<uint32_t>(value);
    }

    isNegative = shouldBeNegative;
    digits.resize(1);
    digits[0] = newValue;

    helperNormalize();
}

void BigInt::assignFromString(const std::string value, bool& success)
{
    assignZero();

    bool hasSeenSignAlready = false;
    BigInt temp;
    BigInt currentMagnitude = BigIntConst::one;

    std::string reversedString = value;
    std::reverse(reversedString.begin(), reversedString.end());

    size_t reversedStringSize = reversedString.size();
    BigInt charBigInt;
    for (size_t i = 0; i < reversedStringSize; i++)
    {
        switch (reversedString[i])
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            if (hasSeenSignAlready)
            {
                success = false;
                assignZero();
                return;
            }
            char characterValue = reversedString[i] - '0';
            charBigInt.assignFromUInt32(characterValue);
            BigInt::multiply(charBigInt, currentMagnitude, temp);
            helperAddUnsigned(temp);
            BigInt::multiply(currentMagnitude, BigIntConst::ten, currentMagnitude);
            break;
        }
        case '+':
        case '-':
        {
            if (hasSeenSignAlready)
            {
                success = false;
                assignZero();
                return;
            }
            hasSeenSignAlready = true;
            isNegative = (reversedString[i] == '-');
            break;
        }
        default:
        {
            success = false;
            assignZero();
            return;
        }
        }
    }

    success = true;
    helperNormalize();
    return;
}

void BigInt::assignZero()
{
    // Guaranteed already normalized (empty digits, sign is false)
    digits.resize(0);
    isNegative = false;
}

uint64_t BigInt::toUInt64(bool& success)
{
    int64_t toReturn = 0;

    if (digits.size() > 2)
    {
        success = false;
        return 0;
    }

    if (digits.size() > 1)
    {
        toReturn |= (static_cast<uint64_t>(digits[1]) << SHIFT_DIGIT_BITS);
    }
    if (digits.size() > 0)
    {
        toReturn |= (static_cast<uint64_t>(digits[0]) << SHIFT_ZERO_BITS);
    }

    success = true;
    return toReturn;
}

int64_t BigInt::toInt64(bool& success)
{
    uint64_t int64_tMaxValueABS = static_cast<uint64_t>(INT64_MAX);
    uint64_t int64_tMinValueABS = static_cast<uint64_t>(INT64_MIN);

    uint64_t unsignedValue = 0;

    if (digits.size() > 2)
    {
        success = false;
        return 0;
    }

    if (digits.size() > 1)
    {
        unsignedValue |= (static_cast<uint64_t>(digits[1]) << SHIFT_DIGIT_BITS);
    }
    if (digits.size() > 0)
    {
        unsignedValue |= (static_cast<uint64_t>(digits[0]) << SHIFT_ZERO_BITS);
    }

    if (unsignedValue <= int64_tMaxValueABS)
    {
        success = true;
        int64_t toReturn = unsignedValue;
        if (isNegative)
        {
            toReturn = -toReturn;
        }
        return toReturn;
    }
    else if (unsignedValue == int64_tMinValueABS && isNegative)
    {
        success = true;
        return INT64_MIN;
    }
    else
    {
        success = false;
        return 0;
    }
}

uint32_t BigInt::toUInt32(bool& success)
{
    int32_t toReturn = 0;

    if (digits.size() > 1)
    {
        success = false;
        return 0;
    }

    if (digits.size() > 0)
    {
        toReturn = digits[0];
    }

    success = true;
    return toReturn;
}

int32_t BigInt::toInt32(bool& success)
{
    uint32_t int32_tMaxValueABS = static_cast<uint32_t>(INT32_MAX);
    uint32_t int32_tMinValueABS = static_cast<uint32_t>(INT32_MIN);

    uint32_t unsignedValue = 0;

    if (digits.size() > 1)
    {
        success = false;
        return 0;
    }

    if (digits.size() > 0)
    {
        unsignedValue = digits[0];
    }

    if (unsignedValue <= int32_tMaxValueABS)
    {
        success = true;
        int32_t toReturn = unsignedValue;
        if (isNegative)
        {
            toReturn = -toReturn;
        }
        return toReturn;
    }
    else if (unsignedValue == int32_tMinValueABS && isNegative)
    {
        success = true;
        return INT32_MIN;
    }
    else
    {
        success = false;
        return 0;
    }
}

std::string BigInt::toString()
{
    if (digits.empty())
    {
        return "0";
    }

    BigInt temp = *this;
    temp.absoluteValue();

    std::string result;

    BigInt remainder;
    while (!temp.isEqualToZero())
    {
        BigInt::modulus(temp, BigIntConst::ten, remainder);
        bool success = false;
        uint32_t digit = remainder.toUInt32(success);
        result += static_cast<char>('0' + digit);

        BigInt::divide(temp, BigIntConst::ten, temp);
    }

    if (isNegative)
    {
        result += '-';
    }

    std::reverse(result.begin(), result.end());

    return result;
}

void BigInt::add(const BigInt& a, const BigInt& b, BigInt& result)
{
    uint8_t bValue = b.isNegative ? 1 : 0;
    uint8_t aValue = a.isNegative ? 1 : 0;
    uint8_t horizontalValue = (aValue << 1) | (bValue << 0);
    uint8_t verticalValue = a.helperCompareToUnsigned(b);
    uint8_t switchValue = (verticalValue << 2) | horizontalValue;

    result.digits = a.digits;
    result.isNegative = a.isNegative;

    switch (switchValue)
    {
    case 0:
    case 3:
    case 4:
    case 7:
    case 8:
    case 11:
    {
        result.helperAddUnsigned(b);
    }
    break;
    case 1:
    case 2:
    {
        result.assignZero();
    }
    break;
    case 5:
    case 6:
    {
        result.helperSubtractReversedUnsigned(b);
        result.isNegative = (switchValue == 5);
    }
    break;
    case 9:
    case 10:
    {
        result.helperSubtractUnsigned(b);
        result.isNegative = (switchValue == 10);
    }
    break;
    default:
    {
        exit(1);
    }
    break;
    }

    result.helperNormalize();
}

void BigInt::subtract(const BigInt& a, const BigInt& b, BigInt& result)
{
    uint8_t bValue = b.isNegative ? 1 : 0;
    uint8_t aValue = a.isNegative ? 1 : 0;
    uint8_t horizontalValue = (aValue << 1) | (bValue << 0);
    uint8_t verticalValue = a.helperCompareToUnsigned(b);
    uint8_t switchValue = (verticalValue * 4) + horizontalValue;

    result.digits = a.digits;
    result.isNegative = a.isNegative;

    switch (switchValue)
    {
    case 1:
    case 2:
    case 5:
    case 6:
    case 9:
    case 10:
    {
        result.helperAddUnsigned(b);
    }
    break;
    case 0:
    case 3:
    {
        result.assignZero();
    }
    break;
    case 4:
    case 7:
    {
        result.helperSubtractReversedUnsigned(b);
        result.isNegative = (switchValue == 4);
    }
    break;
    case 8:
    case 11:
    {
        result.helperSubtractUnsigned(b);
        result.isNegative = (switchValue == 11);
    }
    break;
    default:
    {
        exit(1);
    }
    break;
    }

    result.helperNormalize();
}

void BigInt::multiply(const BigInt& a, const BigInt& b, BigInt& result)
{
    bool shouldBeNegative = (a.isNegative != b.isNegative);

    result.digits = a.digits;
    result.helperMultiplyUnsigned(b);

    result.isNegative = shouldBeNegative;
    result.helperNormalize();
}

void BigInt::divide(const BigInt& a, const BigInt& b, BigInt& result)
{
    bool shouldBeNegative = (a.isNegative != b.isNegative);

    result.digits = a.digits;
    result.helperDivideUnsigned(b);

    result.isNegative = shouldBeNegative;
    result.helperNormalize();
}

void BigInt::modulus(const BigInt& a, const BigInt& b, BigInt& result)
{
    if (b.digits.size() == 0)
    {
        std::abort();
        return;
    }

    bool originalIsNegative = a.isNegative;

    BigInt remainder = a;
    remainder.absoluteValue();

    BigInt divisor = b;
    divisor.absoluteValue();

    BigInt tempDivisor;
    BigInt multiple;
    while (remainder.helperGreaterThanEqualsToUnsigned(divisor))
    {
        tempDivisor = divisor;

        multiple.assignFromUInt32(1);

        while (true)
        {
            tempDivisor.shiftLeft(1);
            if (tempDivisor.helperGreaterThanUnsigned(remainder))
            {
                tempDivisor.shiftRight(1);
                break;
            }

            multiple.shiftLeft(1);
        }

        BigInt::subtract(remainder, tempDivisor, remainder);
    }

    result = remainder;
    result.isNegative = originalIsNegative;
    result.helperNormalize();
}

void BigInt::GCD(const BigInt& a, const BigInt& b, BigInt& result)
{
    if (b.digits.size() == 0)
    {
        result = a;
        result.absoluteValue();
        return;
    }

    if (a.digits.size() == 0)
    {
        result = b;
        result.absoluteValue();
        return;
    }

    BigInt x;
    BigInt y;
    if (a.helperGreaterThanEqualsToUnsigned(b))
    {
        x = a;
        x.absoluteValue();
        y = b;
        y.absoluteValue();
    }
    else
    {
        x = b;
        x.absoluteValue();
        y = a;
        y.absoluteValue();
    }

    BigInt r;
    while (y.digits.size() > 0)
    {
        BigInt::modulus(x, y, r);

        x = y;
        y = r;
    }

    result = x;
    result.helperNormalize();
}

void BigInt::shiftLeft(size_t bits)
{
    if (bits == 0)
    {
        helperNormalize();
        return;
    }

    size_t digitShift = bits / SHIFT_DIGIT_BITS;
    size_t bitShift = bits % SHIFT_DIGIT_BITS;

    if (bitShift == 0)
    {
        digits.insert(digits.begin(), digitShift, 0);
        helperNormalize();
        return;
    }

    size_t originalSize = digits.size();

    digits.resize(originalSize + digitShift + 1, 0);

    size_t i = originalSize;
    while (i > 0)
    {
        i--;

        if (i + digitShift + 1 < digits.size())
        {
            digits[i + digitShift + 1] |= (digits[i] >> (SHIFT_DIGIT_BITS - bitShift));
        }

        digits[i + digitShift] = (digits[i] << bitShift);
    }

    for (size_t i = 0; i < digitShift; i++)
    {
        digits[i] = 0;
    }

    helperNormalize();
}

void BigInt::shiftRight(size_t bits)
{
    if (bits == 0)
    {
        helperNormalize();
        return;
    }

    size_t digitShift = bits / SHIFT_DIGIT_BITS;
    size_t bitShift = bits % SHIFT_DIGIT_BITS;

    if (digitShift >= digits.size())
    {
        assignZero();
        return;
    }

    if (digitShift > 0)
    {
        digits.erase(digits.begin(), digits.begin() + digitShift);
    }

    if (bitShift == 0)
    {
        helperNormalize();
        return;
    }

    size_t size = digits.size();
    for (size_t i = 0; i < size; i++)
    {
        digits[i] = digits[i] >> bitShift;

        if (i + 1 < size)
        {
            digits[i] |= (digits[i + 1] << (32 - bitShift));
        }
    }

    helperNormalize();
}

void BigInt::absoluteValue()
{
    // Guaranteed already normalized (no digit changes)
    isNegative = false;
}

void BigInt::negate()
{
    // Guaranteed already normalized (no digit changes)
    isNegative = !isNegative;
}

uint8_t BigInt::compareTo(const BigInt& other) const
{
    if (digits.size() == 0 && other.digits.size() == 0)
    {
        return VAL_EQUALS;
    }

    if (isNegative != other.isNegative)
    {
        return (isNegative) ? (VAL_LESS_THAN) : (VAL_GREATER_THAN);
    }

    const size_t digitsSize = digits.size();
    const size_t otherDigitsSize = other.digits.size();
    if (digitsSize != otherDigitsSize)
    {
        bool thisHasMoreDigits = digitsSize > otherDigitsSize;
        return (thisHasMoreDigits == isNegative) ? (VAL_LESS_THAN) : (VAL_GREATER_THAN);
    }

    size_t i = digitsSize;
    while (i > 0)
    {
        i--;

        if (digits[i] < other.digits[i])
        {
            return (isNegative) ? (VAL_GREATER_THAN) : (VAL_LESS_THAN);
        }

        if (digits[i] > other.digits[i])
        {
            return (isNegative) ? (VAL_LESS_THAN) : (VAL_GREATER_THAN);
        }
    }

    return VAL_EQUALS;
}

bool BigInt::equals(const BigInt& other) const
{
    return (compareTo(other) == VAL_EQUALS);
}

bool BigInt::lessThan(const BigInt& other) const
{
    return (compareTo(other) == VAL_LESS_THAN);
}

bool BigInt::greaterThan(const BigInt& other) const
{
    return (compareTo(other) == VAL_GREATER_THAN);
}

bool BigInt::notEquals(const BigInt& other) const
{
    return (compareTo(other) != VAL_EQUALS);
}

bool BigInt::lessThanEqualsTo(const BigInt& other) const
{
    return (compareTo(other) != VAL_GREATER_THAN);
}

bool BigInt::greaterThanEqualsTo(const BigInt& other) const
{
    return (compareTo(other) != VAL_LESS_THAN);
}

bool BigInt::isEqualToZero()
{
    return (digits.size() == 0);
}

bool BigInt::isEqualToPositiveOne()
{
    return (digits.size() == 1 && !isNegative && digits[0] == 1);
}

bool BigInt::isEqualToNegativeOne()
{
    return (digits.size() == 1 && isNegative && digits[0] == 1);
}

bool BigInt::getIsNegative()
{
    return isNegative;
}

void BigInt::helperNormalize()
{
    while (digits.size() > 0 && digits.back() == 0)
    {
        digits.pop_back();
    }
}

void BigInt::helperAddUnsigned(const BigInt& other)
{
    bool carry = false;
    size_t index = 0;
    size_t otherSize = other.digits.size();

    while (index < otherSize || carry)
    {
        uint32_t otherDigit = (index < otherSize) ? (other.digits[index]) : (0);

        while (index + 1 > digits.size())
        {
            digits.push_back(0);
        }

        uint32_t oldDigitValue = digits[index];
        uint32_t newDigitValue = oldDigitValue + otherDigit;
        if (carry)
        {
            newDigitValue++;
        }
        digits[index] = newDigitValue;
        carry = (newDigitValue < oldDigitValue);

        index++;
    }
}

void BigInt::helperAddShiftedUInt64Unsigned(uint64_t value, size_t shiftAmmount)
{
    size_t indexOfLower = shiftAmmount + 0;
    size_t indexOfUpper = shiftAmmount + 1;
    bool carry = false;
    size_t index = indexOfLower;
    size_t maxSizeIfNoOverflow = shiftAmmount + 2;

    while (true)
    {
        size_t indexPlusOne = index + 1;

        bool addingDigitFromOther = indexPlusOne <= maxSizeIfNoOverflow;
        if (!(carry || addingDigitFromOther))
        {
            break;
        }

        while (indexPlusOne > digits.size())
        {
            digits.push_back(0);
        }

        if (carry)
        {
            uint32_t oldDigitValue = digits[index];
            uint32_t newDigitValue = oldDigitValue + 1;
            digits[index] = newDigitValue;
            carry = newDigitValue < oldDigitValue;
        }

        if (addingDigitFromOther)
        {
            uint32_t numberToAdd = 0;
            if (index == indexOfLower)
            {
                uint32_t valueOfLower = static_cast<uint32_t>((value >> SHIFT_ZERO_BITS) & UINT32_MAX);
                numberToAdd = valueOfLower;
            }
            else if (index == indexOfUpper)
            {
                uint32_t valueOfUpper = static_cast<uint32_t>((value >> SHIFT_DIGIT_BITS) & UINT32_MAX);
                numberToAdd = valueOfUpper;
            }

            uint32_t oldDigitValue = digits[index];
            uint32_t newDigitValue = oldDigitValue + numberToAdd;
            digits[index] = newDigitValue;
            carry = newDigitValue < oldDigitValue;
        }

        index++;
    }
}

void BigInt::helperSubtractUnsigned(const BigInt& other)
{
    bool carry = false;
    size_t index = 0;
    size_t otherSize = other.digits.size();

    while (index < otherSize || carry)
    {
        uint32_t otherDigit = (index < otherSize) ? (other.digits[index]) : (0);

        while (index + 1 > digits.size())
        {
            digits.push_back(0);
        }

        uint32_t oldDigitValue = digits[index];
        uint32_t newDigitValue = oldDigitValue - otherDigit;
        if (carry)
        {
            newDigitValue--;
        }
        digits[index] = newDigitValue;
        carry = (newDigitValue > oldDigitValue);

        index++;
    }
}

void BigInt::helperSubtractReversedUnsigned(const BigInt& other)
{
    bool carry = false;
    size_t index = 0;
    size_t otherSize = other.digits.size();
    size_t thisSize = digits.size();
    size_t biggestSize = thisSize > otherSize ? thisSize : otherSize;

    digits.resize(biggestSize);

    while (index < otherSize || carry)
    {
        uint32_t otherDigit = (index < otherSize) ? (other.digits[index]) : (0);

        while (index + 1 > digits.size())
        {
            digits.push_back(0);
        }

        uint32_t oldDigitValue = otherDigit;
        uint32_t newDigitValue = oldDigitValue - digits[index];
        if (carry)
        {
            newDigitValue--;
        }
        digits[index] = newDigitValue;
        carry = (newDigitValue > oldDigitValue);

        index++;
    }
}

void BigInt::helperMultiplyUnsigned(const BigInt& other)
{
    const size_t thisSize = digits.size();
    const size_t otherSize = other.digits.size();
    const size_t sumSize = thisSize + otherSize;
    digits.resize(sumSize);

    BigInt oldValue = *this;

    assignZero();

    for (size_t i = 0; i < otherSize; i++)
    {
        for (size_t j = 0; j < thisSize; j++)
        {
            size_t shiftAmount = i + j;
            uint64_t value = static_cast<uint64_t>(other.digits[i]) * static_cast<uint64_t>(oldValue.digits[j]);
            helperAddShiftedUInt64Unsigned(value, shiftAmount);
        }
    }
}

void BigInt::helperDivideUnsigned(const BigInt& other)
{
    if (helperLessThanUnsigned(other))
    {
        assignZero();
        return;
    }

    if (other.digits.size() == 0)
    {
        std::abort();
        return;
    }

    BigInt remainder = *this;
    remainder.absoluteValue();

    assignZero();

    BigInt toAddToAnswerThisRound;
    BigInt toSubtractThisRound;
    BigInt toSubtractThisRoundTimesTwo;
    while (remainder.helperGreaterThanEqualsToUnsigned(other))
    {
        toAddToAnswerThisRound.assignFromUInt32(1);
        toSubtractThisRound = other;
        toSubtractThisRound.absoluteValue();
        toSubtractThisRoundTimesTwo = toSubtractThisRound;
        toSubtractThisRoundTimesTwo.shiftLeft(1);

        while (toSubtractThisRoundTimesTwo.helperLessThanUnsigned(remainder))
        {
            toAddToAnswerThisRound.shiftLeft(1);
            toSubtractThisRound.shiftLeft(1);
            toSubtractThisRoundTimesTwo.shiftLeft(1);
        }

        BigInt::subtract(remainder, toSubtractThisRound, remainder);
        BigInt::add(*this, toAddToAnswerThisRound, *this);
    }
}

uint8_t BigInt::helperCompareToUnsigned(const BigInt& other) const
{
    const size_t digitsSize = digits.size();
    const size_t otherDigitsSize = other.digits.size();
    if (digitsSize != otherDigitsSize)
    {
        bool thisHasMoreDigits = digitsSize > otherDigitsSize;
        return (thisHasMoreDigits) ? (VAL_GREATER_THAN) : (VAL_LESS_THAN);
    }

    size_t i = digitsSize;
    while (i > 0)
    {
        i--;

        if (digits[i] < other.digits[i])
        {
            return VAL_LESS_THAN;
        }

        if (digits[i] > other.digits[i])
        {
            return VAL_GREATER_THAN;
        }
    }

    return VAL_EQUALS;
}

bool BigInt::helperEqualsUnsigned(const BigInt& other) const
{
    return (helperCompareToUnsigned(other) == VAL_EQUALS);
}

bool BigInt::helperLessThanUnsigned(const BigInt& other) const
{
    return (helperCompareToUnsigned(other) == VAL_LESS_THAN);
}

bool BigInt::helperGreaterThanUnsigned(const BigInt& other) const
{
    return (helperCompareToUnsigned(other) == VAL_GREATER_THAN);
}

bool BigInt::helperNotEqualsUnsigned(const BigInt& other) const
{
    return (helperCompareToUnsigned(other) != VAL_EQUALS);
}

bool BigInt::helperLessThanEqualsToUnsigned(const BigInt& other) const
{
    return (helperCompareToUnsigned(other) != VAL_GREATER_THAN);
}

bool BigInt::helperGreaterThanEqualsToUnsigned(const BigInt& other) const
{
    return (helperCompareToUnsigned(other) != VAL_LESS_THAN);
}

BigInt BigIntConst::createOne()
{
    BigInt result;
    result.assignFromUInt32(1);
    return result;
}

BigInt BigIntConst::createTen()
{
    BigInt result;
    result.assignFromUInt32(10);
    return result;
}

const BigInt BigIntConst::one = BigIntConst::createOne();
const BigInt BigIntConst::ten = BigIntConst::createTen();