#include <iostream>
#include <littlesmith/util/Arguments.h>
#include "multirenamer.h"

/**
 * @brief Enum for the 2 rename phases
*/
enum class rename_phase {
    scan,
    rename
};

/**
 * @brief Initializes the arguments object that parses the command line parameters
 *
 * @param arguments The arguments object
*/
void initialize(littlesmith::arguments& arguments);

/**
 * @brief Main function
 *
 * @param argc The number of command line parameters
 * @param argv C-string array containing the command line parameters
 * @returns The result of the operation (0 = no error)
*/
int main(int argc, char* argv[]) {
    littlesmith::arguments arguments;
    initialize(arguments);
    if (!arguments.parse(argc, argv)) {
       return -1;
    }
    arguments.printHeader();
    rename_phase phase;

    auto p = arguments.getValue<std::string>("path");
    std::filesystem::path path;
    if (p.empty()) {
        path = std::filesystem::current_path();
    } else {
        path.assign(p);
    }
    if (arguments.getValue<bool>("scan")) {
        phase = rename_phase::scan;
    } else if (arguments.getValue<bool>("rename")) {
        phase = rename_phase::rename;
    } else {
        std::cerr << "Please specify either --scan or --rename!" << std::endl;
        arguments.printUsage();
        return -1;
    }
    bool recursive = arguments.getValue<bool>("recursive");

    multirenamer renamer(path);
    try {
        if (phase == rename_phase::scan) {
            renamer.scan(recursive);
        } else {
            renamer.rename();
        }
        if (renamer.error()) {
            std::cout << "Some renames failed. See log." << std::endl;
        } else {
            std::cout << "Everything was renamed successfully." << std::endl;
        }
    } catch(std::runtime_error& ex) {
        std::cerr << "Error while renaming:" << std::endl;
        std::cerr << ex.what() << std::endl;
        return -1;
    }
    return 0;
}

void initialize(littlesmith::arguments& arguments) {
    arguments.setDescription("A simple tool to bulk rename _files using your favourite tool (text editor, script, whatever)");
    arguments.setCopyright("ⓒ 2025 by littlesmith");

    arguments.defineSwitch("scan", "s");
    arguments.addDescription("scan", "Scan the rename on a directory");
    arguments.defineSwitch("rename", "r");
    arguments.addDescription("rename", "Perform the rename on a directory");

    arguments.defineValue("path", "p", littlesmith::argument_type::STRING, "", true);
    arguments.addDescription("path", "The path to scan for _files to rename. If omitted, the current directory will be used");
    arguments.defineSwitch("recursive", "R");
    arguments.addDescription("recursive", "Files in subdirectories will also be renamed (only relevant with --scan)");

}
