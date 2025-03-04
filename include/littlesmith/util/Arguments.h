#pragma once

#include <iostream>
#include <map>
#include <filesystem>

#include "littlesmith/util/Exceptions.h"
#include "littlesmith/util/Version.h"
#include "littlesmith/text/Format.h"

namespace littlesmith {

    struct point {
        int32_t x = 0;
        int32_t y = 0;
    };

    struct rectangle {
        point upperLeft {0, 0};
        point lowerRight {0, 0};
    };

    enum class argument_type {
        STRING,
        INT,
        FLOAT,
        BOOL,
        POINT,
        RECTANGLE,
    };

    class argument {
    private:
        std::string _longName;
        std::string _shortName;
        std::string _description;
        argument_type _type;
        std::string _defaultValue;
        bool _switch;
        std::string _value;
        bool _optional;
        bool _set;
        std::string _message;
        void checkType(argument_type type) const;
        void checkValue() const;
        std::string textValue() const;
    public:
        argument(const std::string& longName, const std::string& shortName, argument_type type,
                 const std::string& defaultValue, bool optional, bool isSwitch) :
                _longName(longName), _shortName(shortName),
                _type(type), _defaultValue(defaultValue), _switch(isSwitch),
                _optional(optional), _set(false)
        {  }

        virtual ~argument() = default;
        void setValue(const std::string& value) { _value = value; _set = true; }
        void setDescription(const std::string &description) { _description = description; }
        template<typename T>
        T value() const;
        std::string longName() const { return _longName; }
        std::string shortName() const { return _shortName; }
        std::string message() const { return _message; }
        std::string description() const { return _description; }
        std::string defaultValue() const { return _defaultValue; }
        bool optional() const { return _optional; }
        bool isSwitch() const { return _switch; }
        bool check();
        std::string toString() const;
    };

    class arguments {
    private:
        std::string _application;
        littlesmith::Version _version;
        std::string _copyright;
        std::string _description;
        std::vector<std::string> _keys;
        std::map<std::string, argument> _arguments;
        std::map<std::string, std::string> _longToShortName;
        std::vector<std::string> _messages;
        std::string checkKey(const std::string& key);
        bool _headerPrinted{false};
    public:
        arguments();
        ~arguments() = default;

        bool parse(int argc, char* argv[]);

        void setVersion(const littlesmith::Version& version) { _version = version; }
        void setCopyright(const std::string& copyright) { _copyright = copyright; }
        void setDescription(const std::string& description) { _description = description; }

        void defineValue(const std::string& longName, const std::string& shortName, argument_type type, const std::string& defaultValue = "", bool optional = false);
        void defineSwitch(const std::string& longName, const std::string& shortName);

        void addDescription(const std::string& key, const std::string& description);

        template<typename T>
        T getValue(const std::string& key) { return _arguments.at(checkKey(key)).value<T>(); }
        void printHeader();
        void printUsage();

    };
/*
        STRING,
        INT,
        FLOAT,
        BOOL,
        POS_POINT,
        POS_DIMENSIONS,
        POS_RECTANGLE,
 */

    template<typename T>
    inline T argument::value() const { throw littlesmith::formatException<std::runtime_error>("%s", "No conversion for the given type available");}
    template<>
    inline std::string argument::value() const { checkType(argument_type::STRING); return textValue(); }
    template<>
    inline int argument::value() const { checkType(argument_type::INT); return littlesmith::strto<int>(textValue()); }
    template<>
    inline long argument::value() const { checkType(argument_type::INT); return littlesmith::strto<long>(textValue()); }
    template<>
    inline float argument::value() const { checkType(argument_type::FLOAT); return littlesmith::strto<float>(textValue()); }
    template<>
    inline double argument::value() const { checkType(argument_type::FLOAT); return littlesmith::strto<double>(textValue()); }
    template<>
    inline bool argument::value() const { checkType(argument_type::BOOL); return littlesmith::strto<bool>(textValue()); }
    template<>
    inline point argument::value() const {
        checkType(argument_type::POINT);
        auto p = littlesmith::parsePair<int>(textValue(), "[:]");
        return point((int32_t)p.first, (int32_t)p.second);
    }
    template<>
    inline rectangle argument::value() const {
        checkType(argument_type::RECTANGLE);
        rectangle r;
        auto parts = littlesmith::split(textValue(), "-");
        if (parts.size() != 2) {
            throw std::invalid_argument("wrong format for rectangle");
        }
        try {
            auto p1 = littlesmith::parsePair<int>(parts[0], "[:]");
            r.upperLeft = point((int32_t)p1.first, (int32_t)p1.second);
            auto p2 = littlesmith::parsePair<int>(parts[1], "[:]");
            r.lowerRight = point((int32_t)p2.first, (int32_t)p2.second);
        } catch (std::invalid_argument &ex) {
            throw std::invalid_argument("wrong format for rectangle");
        }
        return r;
    }

    inline bool arguments::parse(int argc, char* argv[]) {
        std::filesystem::path app_path(argv[0]);
        _application = app_path.filename();

        std::vector<std::string> arguments;
        for (int i = 1; i < argc; i++) {
            arguments.emplace_back(argv[i]);
        }
        std::string key;
        std::string value;
        //0 = expecting key
        //1 = expecting value
        //2 = collecting value
        //3 = got value
        int status = 0;
        for (const auto& arg : arguments) {
            if (status == 0) {
                key = arg;
                if (arg.find('=') != std::string::npos) {
                    auto parts = littlesmith::split(arg, "=");
                    key = parts[0];
                    value = parts[1];
                    if (value[0] == '"') {
                        status = 2;
                    } else {
                        status = 3;
                    }
                }
                if (key.substr(0, 2) == "--") {
                    key = key.substr(2);
                }
                else if (key.substr(0, 1) == "-") {
                    key = key.substr(1);
                }
                try {
                    key = checkKey(key);
                    if (_arguments.at(key).isSwitch()) {
                        value = "true";
                        status = 3;
                    }
                    if (status == 0) {
                        status = 1;
                    }
                }
                catch(std::invalid_argument& ex) {
                    _messages.emplace_back(ex.what());
                    status = 0;
                }
            } else if (status == 1) {
                value = arg;
                if (value[0] == '"') {
                    status = 2;
                } else {
                    status = 3;
                }
            } else {
                value = " " + arg;
                if (value.substr(value.length() - 1) == "\"" && value.substr(value.length() - 2) != "\\") {
                    status = 3;
                }
            }
            if (status == 3) {
                _arguments.at(key).setValue(value);
                status = 0;
            }
        }
        if (status != 0) {
            _messages.emplace_back("Bad argument format.");
        }
        for (auto& kvp : _arguments) {
            if (!kvp.second.check()) {
                _messages.emplace_back(kvp.second.message());
            }
        }
        if (!_messages.empty() || getValue<bool>("h")) {
            printUsage();
            return false;
        }
        return true;
    }

    inline void arguments::defineValue(const std::string& longName, const std::string& shortName, argument_type type,
                                       const std::string& defaultValue, bool optional) {
        if (_arguments.contains(shortName)) {
            throw littlesmith::formatException<std::invalid_argument>("Argument with shortname '%s' already defined.", shortName.c_str());
        }
        if (_longToShortName.contains(longName)) {
            throw littlesmith::formatException<std::invalid_argument>("Argument with longname '%s' already defined.", longName.c_str());
        }
        _arguments.emplace(shortName, argument(longName, shortName, type, defaultValue, optional, false));
        _longToShortName.emplace(longName, shortName);
        _keys.emplace_back(shortName);
    }

    inline void arguments::defineSwitch(const std::string& longName, const std::string& shortName) {
        if (_arguments.contains(shortName)) {
            throw littlesmith::formatException<std::invalid_argument>("Argument with shortname '%s' already defined.", shortName.c_str());
        }
        if (_longToShortName.contains(longName)) {
            throw littlesmith::formatException<std::invalid_argument>("Argument with longname '%s' already defined.", longName.c_str());
        }
        _arguments.emplace(shortName, argument(longName, shortName, argument_type::BOOL, "false", true, true));
        _longToShortName.emplace(longName, shortName);
        _keys.emplace_back(shortName);
    }

    inline void argument::checkType(argument_type type) const {
        if (_type != type) {
            throw std::invalid_argument("invalid type");
        }
    }

    inline void argument::checkValue() const {
        switch (_type) {
            case argument_type::STRING:
                value<std::string>();
                break;
            case argument_type::INT:
                value<long>();
                break;
            case argument_type::FLOAT:
                value<double>();
                break;
            case argument_type::BOOL:
                value<bool>();
                break;
            case argument_type::POINT:
                value<point>();
                break;
            case argument_type::RECTANGLE:
                value<rectangle>();
                break;
        }
    }

    inline bool argument::check() {
        if (!_set) {
            if (!_optional) {
                _message = "Argument '" + _longName + "' is not optional.";
                return false;
            }
        }
        try {
            checkValue();
        } catch(std::invalid_argument& ex) {
            _message = "Value for argument '" + _longName + "' could not be parsed.";
            return false;
        } catch(std::out_of_range& ex) {
            _message = "Value for argument '" + _longName + "' is out of range.";
            return false;
        }
        return true;
    }

    inline std::string argument::textValue() const {
        if (_set) {
            return _value;
        }
        return _defaultValue;
    }

    inline std::string argument::toString() const {
        std::stringstream ss;
        if (_optional) {
            ss << "[";
        }
        ss << "{-" << _shortName << "|--" << _longName << "}";
        if (!_switch) {
            ss << "[=]";
            if (_optional) {
                ss << _defaultValue;
            } else {
                ss << "(value)";
            }
        }
        if (_optional) {
            ss << "]";
        }
        return ss.str();
    }

    inline std::string arguments::checkKey(const std::string &key) {
        std::string result;
        if (_arguments.contains(key)) {
            result = key;
        } else {
            try {
                result = _longToShortName.at(key);
            } catch (std::out_of_range& ex) {
                throw littlesmith::formatException<std::invalid_argument>("Undefined argument '%s'", key.c_str());
            }
        }
        return result;
    }

    inline void arguments::addDescription(const std::string &key, const std::string &description) {
        argument &arg = _arguments.at(checkKey(key));
        arg.setDescription(description);
    }
    inline void arguments::printHeader() {
        if (_headerPrinted) return;
        std::cout << _application << " " << _version.toString() << std::endl;
        if (!_copyright.empty()) {
            std::cout << _copyright << std::endl;
        }
        std::cout << std::endl;
        if (!_description.empty()) {
            auto lines = littlesmith::to_block(_description);
            for (const auto& line : lines) {
                std::cout << line << std::endl;
            }
            std::cout << std::endl;
        }
        _headerPrinted = true;
    }

    inline void arguments::printUsage() {
        printHeader();
        if (!_messages.empty()) {
            std::cout << "Error" << (_messages.size() > 1 ? "s" : "") << ":" << std::endl;
            for (const auto& message : _messages) {
                std::cout << message << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << "Usage:" << std::endl;
        std::cout << _application;
        std::string indent = std::string(_application.length(), ' ');
        size_t n = indent.length();
        for (const auto& key : _keys) {
            argument &arg = _arguments.at(key);
            std::string a = arg.toString();
            if (n + a.length() + 1 > 80) {
                std::cout << std::endl << indent;
                n += indent.length();
            }
            std::cout << " " << a;
            n += a.length() + 1;
        }
        std::cout << std::endl << std::endl;
        size_t max = 0;
        for (const auto& key : _keys) {
            argument &arg = _arguments.at(key);
            size_t l =arg.longName().length() + arg.shortName().length();
            if (l > max) {
                max = l;
            }
        }
        max += 8;
        indent = std::string(max, ' ');
        for (const auto& key : _keys) {
            argument &arg = _arguments.at(key);
            auto lines = littlesmith::to_block(arg.description(), 80 - max);
            std::string argStr = "--" + arg.longName() + " | -" + arg.shortName() + ": ";
            argStr += std::string(max - argStr.length(), ' ');
            std::cout << argStr << lines[0] << std::endl;
            for (size_t i = 1; i < lines.size(); i++) {
                std::cout << indent << lines[i] << std::endl;
            }
            if (arg.optional() && !arg.isSwitch()) {
                std::cout << indent << "optional";
                if (!arg.defaultValue().empty()) {
                    std::cout << " - default: " << arg.defaultValue();
                }
                std::cout << std::endl;
            }
        }

    }

    inline arguments::arguments() : _version(1, 0, 0) {
        defineSwitch("help", "h");
        addDescription("h", "Show this message");
    }


} // engine::backends
