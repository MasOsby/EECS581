#include <iostream>
#include <string>
#include <cctype>

bool extractIPv4(const std::string& str, unsigned long& outAddress, int& outPort)
{
    outAddress = 0;
    outPort = -1;

    size_t start = 0;

    while (start < str.length())
    {
        // Skip garbage characters
        if (!isdigit(str[start]))
        {
            start++;
            continue;
        }

        // Find the entire candidate token.
        // Digits, periods, and colons are considered part of a candidate.
        size_t end = start;

        while (end < str.length() &&
              (isdigit(str[end]) || str[end] == '.' || str[end] == ':'))
        {
            end++;
        }

        size_t i = start;
        int octets[4];
        bool valid = true;

        for (int part = 0; part < 4; part++)
        {
            if (i >= end || !isdigit(str[i]))
            {
                valid = false;
                break;
            }

            int value = 0;
            int digits = 0;

            if (str[i] == '0' &&
                i + 1 < end &&
                isdigit(str[i + 1]))
            {
                valid = false;
                break;
            }

            while (i < end && isdigit(str[i]))
            {
                // More than 3 digits in an octet
                if (digits >= 3)
                {
                    valid = false;
                    break;
                }

                value = value * 10 + (str[i] - '0');
                i++;
                digits++;
            }

            if (!valid || value > 255)
            {
                valid = false;
                break;
            }

            octets[part] = value;

            if (part < 3)
            {
                if (i >= end || str[i] != '.')
                {
                    valid = false;
                    break;
                }

                i++;
            }
        }

        int port = -1;

        if (valid && i < end && str[i] == ':')
        {
            i++;

            if (i >= end || !isdigit(str[i]))
            {
                valid = false;
            }
            else
            {
                int value = 0;
                int digits = 0;

                if (str[i] == '0' &&
                    i + 1 < end &&
                    isdigit(str[i + 1]))
                {
                    valid = false;
                }

                while (valid && i < end && isdigit(str[i]))
                {
                    if (digits >= 5)
                    {
                        valid = false;
                        break;
                    }

                    value = value * 10 + (str[i] - '0');
                    i++;
                    digits++;
                }

                if (valid && value > 65535)
                    valid = false;

                if (valid)
                    port = value;
            }
        }

        // Anything remaining means the entire candidate was malformed.
        if (valid && i != end)
            valid = false;

        if (valid)
        {
            outAddress =
                ((unsigned long)octets[0] << 24) |
                ((unsigned long)octets[1] << 16) |
                ((unsigned long)octets[2] << 8) |
                (unsigned long)octets[3];

            outPort = port;
            return true;
        }

        // Skip the entire invalid candidate rather than trying
        // to extract a valid substring from inside of it.
        start = end;
    }

    return false;
}

int main()
{
    std::string input;

    while (true)
    {
        std::cout << "Enter a string (or 'END' to quit): ";
        std::getline(std::cin, input);

        if (input == "END")
        {
            std::cout << "Program terminated." << std::endl;
            break;
        }

        unsigned long address;
        int port;

        if (extractIPv4(input, address, port))
        {
            unsigned long a = (address >> 24) & 255;
            unsigned long b = (address >> 16) & 255;
            unsigned long c = (address >> 8) & 255;
            unsigned long d = address & 255;

            std::cout << "Extracted IPv4 address: "
                      << a << "." << b << "." << c << "." << d
                      << " (decimal value: " << address
                      << ", port: ";

            if (port == -1)
                std::cout << "none";
            else
                std::cout << port;

            std::cout << ")" << std::endl;
        }
        else
        {
            std::cout << "Invalid input: no valid IPv4 address found"
                      << std::endl;
        }
    }

    return 0;
}