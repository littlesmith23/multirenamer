/**
* @file multirenamer.cpp
 * @author Stefan Kleinschmiodt
 * @date 27. Jan 2025
 * @brief Contains the definition of the multirenamer class.
 *
 * Provides functionality for multiple renaming of files
 */


#pragma once
#include <filesystem>

/**
 * @brief Class containing the implementation of multirenamer
 *
 * The class provides 2 methods scan and rename for the two
 * phases of the tool.
 *
*/
class multirenamer {

private:
    std::filesystem::path _path;
    std::filesystem::path _rename_txt;
    std::filesystem::path _old_name_txt;
    bool _logged{false};

    std::vector<std::filesystem::path> _files;

public:
    /**
     * @brief Constructor for the multirenamer
     *
     * @param path The directory to work in
    */
    explicit multirenamer(const std::filesystem::path& path);



    [[nodiscard]] bool error() const { return _logged; }

    /**
     * @brief Scans the given path and writes the rename file
     *
     * @param recursive If true, also scan subdirectories recursively.
    */
    void scan(bool recursive);
    /**
     * @brief Reads the rename file and performs the renaming and moving.
    */
    void rename();
};
