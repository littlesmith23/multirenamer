//
// Created by stefan on 21.04.23.
//

#pragma once
#include "../text/Format.h"

namespace littlesmith {

    class Version {
    private:
        unsigned short _major;
        unsigned short _minor;
        unsigned int _revision;

    public:
        Version() :
                _major(0),
                _minor(0),
                _revision(0)
        {}

        Version(unsigned short major, unsigned short minor, unsigned int revision) :
            _major(major),
            _minor(minor),
            _revision(revision)
        {}

        explicit Version(unsigned long bytes) {
            _revision = bytes & 0xffffffff;
            bytes = bytes >> 32;
            _minor = bytes & 0xffff;
            bytes = bytes >> 16;
            _major = bytes & 0xffff;
        }

        explicit Version(std::string version) {
            size_t idx;
            _major = littlesmith::strto<unsigned short>(version, &idx);
            version = version.substr(idx + 1);
            _minor = littlesmith::strto<unsigned short>(version, &idx);
            version = version.substr(idx + 1);
            _revision = littlesmith::strto<unsigned int>(version);
        }

        unsigned short major() const { return _major; }
        unsigned short minor() const { return _minor; }
        unsigned int revision() const { return _revision; }

        std::string toString() const {
            std::stringstream ss;
            ss << _major << "." << _minor << "." << _revision;
            return ss.str();
        }

        unsigned long toBytes() const {
            unsigned long bytes = _major;
            bytes = bytes << 16;
            bytes += _minor;
            bytes = bytes << 32;
            bytes += _revision;
            return bytes;
        }
    };
}