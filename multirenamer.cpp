/**
* @file multirenamer.cpp
 * @author Stefan Kleinschmiodt
 * @date 27. Jan 2025
 * @brief Contains the implementations of the multirenamer class.
 *
 * Provides functionality for multiple renaming of files
 */

#include <vector>
#include <stack>
#include <fstream>

#include "multirenamer.h"
#include <littlesmith/crypto/SHA256.h>

multirenamer::multirenamer(const std::filesystem::path &path)  :
    _path(path), _rename_txt(path), _old_name_txt(std::filesystem::temp_directory_path()) {
    _old_name_txt.append(".multirenamer_name_list_" + littlesmith::SHA256::hashString(path) + ".txt");
    _rename_txt.append("multirenamer.txt");
}

void multirenamer::scan(bool recursive) {
    std::stack<std::filesystem::path> stack;
    stack.push(_path);

    std::ofstream rename(_rename_txt);
    std::ofstream old_name(_old_name_txt);
    while (!stack.empty()) {
        auto current = stack.top();
        stack.pop();
        for (auto const &entry: std::filesystem::directory_iterator(current, std::filesystem::directory_options::skip_permission_denied)) {
            if (entry.is_regular_file()) {
                if (entry.path().filename() != _rename_txt.filename() &&
                        entry.path().filename() != _old_name_txt.filename()) {
                    auto name = entry.path().string();
                    if (name.starts_with('"') && name.ends_with('"')) {
                        name = name.substr(1, name.length() - 2);
                    }
                    rename << name << std::endl;
                    old_name << name << std::endl;
                }
            }
            if (entry.is_directory() && recursive) {
                stack.push(entry.path());
            }
        }
    }
}

void multirenamer::rename() {
    if (!std::filesystem::exists(_rename_txt)) {
        throw std::runtime_error("No rename file found on this path!");
    }
    if (!std::filesystem::exists(_old_name_txt)) {
        throw std::runtime_error("No old name file found on this path!");
    }
    std::ifstream rename(_rename_txt);
    std::ifstream old_name(_old_name_txt);
    std::string newName, oldName;
    auto log_path = _path;
    log_path.append("multirenamer_error.log");
    if (std::filesystem::exists(log_path)) {
        std::filesystem::remove(log_path);
    }
    std::ofstream log_file;
    _logged = false;
    while (std::getline(old_name, oldName)) {
        if (!std::getline(rename, newName)) {
            throw std::runtime_error("Could not read new name from rename file!");
        }
        if (oldName != newName) {
            try {
                auto oldPath = std::filesystem::path(oldName).parent_path();
                auto newPath = std::filesystem::path(newName).parent_path();
                if (oldPath != newPath) {
                    std::filesystem::create_directories(newPath);
                }
                std::filesystem::rename(oldName, newName);
            } catch (std::filesystem::filesystem_error &ex) {
                if (!_logged) {
                    log_file.open(log_path);
                    _logged = true;
                }
                log_file << "Failed to rename: " << std::endl;
                log_file << "  " << oldName << std::endl << " - " << newName << std::endl;
                log_file << "  Error:" << ex.what() << std::endl << std::endl;
            }
        }
    }
    if (_logged) {
        log_file.close();
    }
    std::filesystem::remove(_old_name_txt);
    auto renamed_txt = std::filesystem::path(_path).append("multirenamer_renamed.txt");
    std::filesystem::copy(_rename_txt, renamed_txt);
    std::filesystem::remove(_rename_txt);
}
