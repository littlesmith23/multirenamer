//
// Created by stefan on 20.04.23.
//

#pragma once
#include <string>
#include <sstream>
#include "String.h"
#include "../util/Exceptions.h"
namespace littlesmith {

    template <typename T>
    T strto(const std::string& str, size_t* idx = nullptr);


    template <typename T>
    inline T strto(const std::string& str, size_t* idx) { *idx = 0; throw formatException<std::invalid_argument>("strto(\"%s\") is not implemented for this type", str.c_str()); }
    template <>
    inline short strto(const std::string& str, size_t* idx) {
        int n;
        try {
            n = std::stoi(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<short>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<short>");
        }
        if (n < INT16_MIN || n > INT16_MAX) {
            throw std::out_of_range("strto<short>");
        }
        return n;
    }
    template <>
    inline int strto(const std::string& str, size_t* idx) {
        try {
            return std::stoi(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<int>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<int>");
        }
    }
    template <>
    inline long strto(const std::string& str, size_t* idx) {
        try {
            return std::stol(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<long>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<long>");
        }
    }
    template <>
    inline long long strto(const std::string& str, size_t* idx) {
        try {
            return std::stoll(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<long long>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<long long>");
        }
    }
    template <>
    inline unsigned int strto(const std::string& str, size_t* idx) {
        unsigned int n;
        try {
            n = std::stoul(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<unsigned ini>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<unsigned ini>");
        }
        if (n > UINT32_MAX) {
            throw std::out_of_range("strto<unsigned int>");
        }
        return (unsigned int)n;
    }
    template <>
    inline unsigned short strto(const std::string& str, size_t* idx) {
        unsigned int n;
        try {
            n = strto<unsigned int>(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<unsigned short>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<unsigned short>");
        }
        if (n > UINT16_MAX) {
            throw std::out_of_range("strto<unsigned short>");
        }
        return n;
    }
    template <>
    inline unsigned long strto(const std::string& str, size_t* idx) {
        try {
            return std::stoul(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<unsigned long>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<unsigned long>");
        }
    }
    template <>
    inline unsigned long long strto(const std::string& str, size_t* idx) {
        try {
            return std::stoull(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<unsigned long long>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<unsigned long long>");
        }
    }
    template <>
    inline float strto(const std::string& str, size_t* idx) {
        try {
            return std::stof(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<float>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<float>");
        }
    }
    template <>
    inline double strto(const std::string& str, size_t* idx) {
        try {
            return std::stod(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<double>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<double>");
        }
    }
    template <>
    inline long double strto(const std::string& str, size_t* idx) {
        try {
            return std::stold(str, idx);
        } catch(std::invalid_argument& ex) {
            throw std::invalid_argument("strto<long double>");
        } catch(std::out_of_range& ex) {
            throw std::out_of_range("strto<long double>");
        }
    }
    template <>
    inline bool strto(const std::string& str, size_t* idx) {
        std::string s = to_lower(str);
        if (s == "yes" || s == "on" || s == "true") {
            if (idx != nullptr) *idx = s.length();
            return true;
        }
        if (s == "no" || s == "off" || s == "false") {
            if (idx != nullptr) *idx = s.length();
            return false;
        }
        throw std::invalid_argument("strto<bool>");
    }

    template <typename T>
    inline std::pair<T, T> parsePair(const std::string& text, std::string format) {
        if (format.length() != 3) {
            throw std::invalid_argument("invalid pair format");
        }
        std::string t = text;
        if (t[0] != format[0]) {
            throw std::invalid_argument("wrong or missing opening bracket");
        }
        t = t.substr(1);
        size_t idx = 0;
        int x = strto<int>(t, &idx);
        t = t.substr(idx);
        if (t[0] != format[1]) {
            throw std::invalid_argument("wrong or missing separator");
        }
        t = t.substr(1);
        int y = strto<int>(t, &idx);
        if (t[idx] != format[2]) {
            throw std::invalid_argument("wrong or missing closing bracket");
        }
        return { x, y};
    }

    template <typename T>
    inline std::string formatPair(std::pair<T, T> pair, std::string format) {
        if (format.length() != 3) {
            throw std::invalid_argument("invalid pair format");
        }

        std::stringstream text;
        text << format[0];
        text << pair.first;
        text << format[1];
        text << pair.second;
        text << format[2];
        return text.str();
    }

}