//
// Created by stefan on 20.04.23.
//
#pragma once
#include <string>
#include <sstream>
#include <vector>
#include <deque>
#include <algorithm>
#include "../util/Compat.h"

namespace littlesmith {
    inline std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> result;
        std::string s = str;
        size_t pos;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            result.emplace_back(token);
            s.erase(0, pos + delimiter.length());
        }
        if (!s.empty()) {
            result.emplace_back(s);
        }
        return result;
    }

    inline size_t findFirstOf(const std::string& haystack, const std::vector<std::string>& needles, std::string& foundDelimiter) {
        size_t pos = std::string::npos;

        for (const auto& needle : needles) {
            size_t p = haystack.find(needle);
            if (p != std::string::npos) {
                if (pos == std::string::npos || p < pos) {
                    foundDelimiter = needle;
                    pos = p;
                }
            }
        }
        return pos;
    }

    inline std::vector<std::string> split(std::string str, const std::vector<std::string>& delimiters, std::vector<std::string>& delimiterList) {
        std::vector<std::string> result;
        size_t pos;
        std::string token;
        std::string foundDelimiter;
        while ((pos = findFirstOf(str, delimiters, foundDelimiter)) != std::string::npos) {
            token = str.substr(0, pos);
            result.emplace_back(token);
            delimiterList.emplace_back(foundDelimiter);
            str.erase(0, pos + foundDelimiter.length());
        }
        if (!str.empty()) {
            result.emplace_back(str);
        }
        return result;
    }

    inline std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
        std::stringstream ss;
        std::deque<std::string> q(strings.begin(), strings.end());
        ss << q.front();
        q.pop_front();
        while(!q.empty()) {
            ss << delimiter << q.front();
            q.pop_front();
        }
        return ss.str();
    }

    inline std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c){ return std::tolower(c); });
        return result;
    }

    inline std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c){ return std::toupper(c); });
        return result;
    }

    enum class text_align {
        left,
        center,
        right
    };

    inline std::string align_text(std::string line, unsigned int width, text_align align, bool padRight = false) {
        size_t lineWidth = line.length();
        if (lineWidth < width) {
            unsigned int diff = width - lineWidth;
            unsigned int pad1 = 0;
            unsigned int pad2 = 0;
            if (align == text_align::left) {
                pad2 = diff;
            }
            else if (align == text_align::center) {
                pad1 = diff / 2;
                pad2 = pad1 + (diff % 2);
            }
            else if (align == text_align::right) {
                pad1 = diff;
            }
            if (pad1 > 0) {
                line = std::string(pad1, ' ') + line;
            }
            if (pad2 > 0 && padRight) {
                line += std::string(pad2, ' ');
            }
        }
        return line;
    }

    inline std::vector<std::string> to_block(const std::string& str, unsigned int width = 80, text_align align = text_align::left) {
        std::vector<std::string> block;

        std::vector<std::string> rawDelimiterList;

        std::vector<std::string> rawParts = split(str, {" ", LINEFEED, "\t"}, rawDelimiterList);
        std::string currentLine;
        std::vector<std::string> parts;
        std::vector<std::string> delimiterList;
        for (size_t i = 0; i < rawParts.size(); i++) {
            std::string part = rawParts[i];
            while (part.length() > width) {
                parts.emplace_back(part.substr(0, width));
                part = part.substr(width);
                if (!part.empty()) {
                    delimiterList.emplace_back(LINEFEED);
                }
            }
            if (!part.empty()) {
                parts.emplace_back(part);
            }
            if (i < rawDelimiterList.size()) {
                delimiterList.emplace_back(rawDelimiterList[i]);
            }
        }

        for (size_t i = 0; i < parts.size(); i++) {
            bool newLine = false;
            if (currentLine.length() + parts[i].length() <= width) {
                currentLine += parts[i];
                if (i < delimiterList.size()) {
                    if (delimiterList[i] == LINEFEED) {
                        newLine = true;
                    }
                    else {
                        if (currentLine.length() + delimiterList[i].length() <= width) {
                            currentLine += delimiterList[i];
                        } else {
                            newLine = true;
                        }
                    }
                }
            } else {
                i -= 1;
                newLine = true;
            }
            if (newLine) {
                block.emplace_back(align_text(currentLine, width, align));
                currentLine = "";
            }
        }
        if (!currentLine.empty()) {
            block.emplace_back(align_text(currentLine, width, align));
        }
        return block;
    }
}