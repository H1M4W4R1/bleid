#include "uuid_generator.h"

#include <cctype>
#include <sstream>
#include <stdexcept>

const char *DEFAULT_BASE_UUID = "ae615SSS-TTTT-4RRR-8000-XXXXXXXXXXXX";

namespace
{
const unsigned long long XOR_PHRASE = 0x123456789ABCULL;
const unsigned long long PHRASE_MASK = 0xFFFFFFFFFFFFULL;

bool is_hex_char(char c)
{
    return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

bool is_placeholder_char(char c)
{
    const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
    return upper == 'S' || upper == 'T' || upper == 'R' || upper == 'X';
}

std::string to_lower(std::string value)
{
    for (std::string::size_type i = 0; i < value.size(); ++i)
    {
        value[i] = static_cast<char>(std::tolower(static_cast<unsigned char>(value[i])));
    }
    return value;
}

std::string raw_hex(unsigned long long value)
{
    std::ostringstream out;
    out << std::hex << std::nouppercase << value;
    return out.str();
}

std::string left_pad_hex(const std::string &value, std::string::size_type width)
{
    if (value.size() >= width)
    {
        return value;
    }
    return std::string(width - value.size(), '0') + value;
}

std::string hex_for_index(unsigned long long value, std::string::size_type width, const char *name)
{
    if (width == 0)
    {
        throw std::runtime_error(std::string("base UUID has no ") + name + " placeholders");
    }

    const std::string hex = raw_hex(value);
    if (hex.size() > width)
    {
        throw std::runtime_error(std::string(name) + " does not fit in base UUID placeholders");
    }

    return left_pad_hex(hex, width);
}

std::string hex_for_hash(unsigned long long value, std::string::size_type width)
{
    if (width == 0)
    {
        throw std::runtime_error("base UUID must include at least one X hash placeholder");
    }

    const std::string hex = raw_hex(value);
    if (hex.size() > width)
    {
        return hex.substr(hex.size() - width);
    }

    return left_pad_hex(hex, width);
}

std::string::size_type count_placeholders(const std::string &base, char placeholder)
{
    std::string::size_type count = 0;
    for (std::string::size_type i = 0; i < base.size(); ++i)
    {
        const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(base[i])));
        if (upper == placeholder)
        {
            ++count;
        }
    }
    return count;
}

void replace_placeholders(std::string &base, char placeholder, const std::string &replacement)
{
    std::string::size_type index = 0;
    for (std::string::size_type i = 0; i < base.size(); ++i)
    {
        const char upper = static_cast<char>(std::toupper(static_cast<unsigned char>(base[i])));
        if (upper == placeholder)
        {
            base[i] = replacement[index++];
        }
    }
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

        if (!is_hex_char(base[i]) && !is_placeholder_char(base[i]))
        {
            throw std::runtime_error("base UUID may only contain hex digits, S/T/R/X placeholders, and hyphens");
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

    const std::string::size_type service_width = count_placeholders(base, 'S');
    const std::string::size_type characteristic_width = count_placeholders(base, 'T');
    const std::string::size_type descriptor_width = count_placeholders(base, 'R');
    const std::string::size_type hash_width = count_placeholders(base, 'X');

    if (has_service || service_width > 0)
    {
        replace_placeholders(base, 'S', hex_for_index(service, service_width, "service"));
    }

    if (has_characteristic || characteristic_width > 0)
    {
        replace_placeholders(base, 'T', hex_for_index(characteristic, characteristic_width, "characteristic"));
    }

    if (has_descriptor || descriptor_width > 0)
    {
        replace_placeholders(base, 'R', hex_for_index(descriptor, descriptor_width, "descriptor"));
    }

    const unsigned long long suffix = XOR_PHRASE ^ hexified_phrase(phrase);
    replace_placeholders(base, 'X', hex_for_hash(suffix, hash_width));
    return to_lower(base);
}
