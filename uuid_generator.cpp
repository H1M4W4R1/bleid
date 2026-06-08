#include "uuid_generator.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

const char *DEFAULT_BASE_UUID = "ae615000-0000-4000-8000-XXXXXXXXXXXX";

namespace
{
const unsigned long long XOR_PHRASE = 0x123456789ABCULL;
const unsigned long long PHRASE_MASK = 0xFFFFFFFFFFFFULL;

bool is_hex_char(char c)
{
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

std::string to_lower(std::string value)
{
    for (std::string::size_type i = 0; i < value.size(); ++i)
    {
        value[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(value[i])));
    }
    return value;
}

std::string hex_width(unsigned long long value, int width)
{
    std::ostringstream out;
    out << std::hex << std::nouppercase << std::setw(width) << std::setfill('0') << value;
    return out.str();
}

void validate_base_uuid(const std::string &base)
{
    if (base.size() != 36)
    {
        throw std::runtime_error("base UUID must be 36 characters long");
    }

    const int hyphen_positions[] = {8, 13, 18, 23};
    for (int i = 0; i < 4; ++i)
    {
        if (base[hyphen_positions[i]] != '-')
        {
            throw std::runtime_error("base UUID must use UUID hyphen positions");
        }
    }

    for (std::string::size_type i = 0; i < base.size(); ++i)
    {
        if (base[i] == '-')
        {
            continue;
        }

        if (!is_hex_char(base[i]) && base[i] != 'x' && base[i] != 'X')
        {
            throw std::runtime_error("base UUID may only contain hex digits, x placeholders, and hyphens");
        }
    }
}

unsigned long long hexified_phrase(const std::string &phrase)
{
    unsigned long long value = 0;
    for (std::string::size_type i = 0; i < phrase.size(); ++i)
    {
        const unsigned long long byte = static_cast<unsigned char>(phrase[i]);
        if (i < 6)
        {
            value = (value << 8) | byte;
        }
        else
        {
            const int shift = static_cast<int>((5 - (i % 6)) * 8);
            value ^= byte << shift;
        }
    }

    if (phrase.size() < 6)
    {
        value <<= static_cast<int>((6 - phrase.size()) * 8);
    }

    return value & PHRASE_MASK;
}
}

unsigned long long parse_unsigned(const std::string &text, unsigned long long maximum, const char *name)
{
    if (text.empty())
    {
        throw std::runtime_error(std::string(name) + " cannot be empty");
    }

    int base = 10;
    std::string digits = text;
    if (digits.size() > 2 && digits[0] == '0' && (digits[1] == 'x' || digits[1] == 'X'))
    {
        base = 16;
        digits = digits.substr(2);
    }

    if (digits.empty())
    {
        throw std::runtime_error(std::string(name) + " has no digits");
    }

    unsigned long long value = 0;
    for (std::string::size_type i = 0; i < digits.size(); ++i)
    {
        int digit = -1;
        const char c = digits[i];
        if (c >= '0' && c <= '9')
        {
            digit = c - '0';
        }
        else if (base == 16 && c >= 'a' && c <= 'f')
        {
            digit = 10 + c - 'a';
        }
        else if (base == 16 && c >= 'A' && c <= 'F')
        {
            digit = 10 + c - 'A';
        }

        if (digit < 0 || digit >= base)
        {
            throw std::runtime_error(std::string(name) + " contains invalid digits");
        }

        if (value > (maximum - static_cast<unsigned long long>(digit)) / static_cast<unsigned long long>(base))
        {
            throw std::runtime_error(std::string(name) + " is out of range");
        }
        value = value * static_cast<unsigned long long>(base) + static_cast<unsigned long long>(digit);
    }

    return value;
}

std::string generate_uuid(std::string base,
                          const std::string &phrase,
                          bool has_service,
                          unsigned int service,
                          bool has_characteristic,
                          unsigned int characteristic,
                          bool has_descriptor,
                          unsigned int descriptor)
{
    validate_base_uuid(base);
    base = to_lower(base);

    if (has_service)
    {
        base.replace(5, 3, hex_width(service, 3));
    }

    if (has_characteristic)
    {
        base.replace(9, 4, hex_width(characteristic, 4));
    }

    if (has_descriptor)
    {
        base.replace(15, 3, hex_width(descriptor, 3));
    }

    const unsigned long long suffix = XOR_PHRASE ^ hexified_phrase(phrase);
    base.replace(24, 12, hex_width(suffix, 12));
    return base;
}
