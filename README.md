# About multirenamer

multirenamer is a very simple tool that enables the user to bulk rename files in a directory, optionally including subdirectories.

multirenamer runs in two phases: scan and rename.

In the scan phase the tool scans the directory (and its subdirectories, if stated) and lists all files with complete path in the file multirenamer.txt.

Then you can open this file in any tool you want (text editor, script or what ever) and change all file names as you like.

After that run the rename phase. The tool will rename the files according to the changes in your file.

If you like this little tool and want to give something back, please send bug-reports or add PRs with bug fixes.

<b>Please note: This is a hobby project, created just for fun, so do not expect the reaction speed of a full-time development team.</b>

# Usage

multirenamer \[{-h|--help}] \[{-s|--scan}] [{-r|--rename}] \[{-p|--path}[=]]
[{-R|--recursive}]

--help | -h:      Show this message  
--scan | -s:      Scan the rename on a directory  
--rename | -r:    Perform the rename on a directory  
--path | -p:      The path to scan for _files to rename. If omitted, the current  directory will be used

## Example
### Scan
```bash
multirename --scan --path /home/user/docs/files/ --recursive 
```
Scans the directory /home/user/docs/files/ recursively and stores the file list in  
/home/user/docs/files/multirenamer.txt
### Editing
Now you can edit multirenamer.txt.
Each line contains a filename with the full path. Change the file names and paths as you wish.
If you have finished editing the file, save it.

### Rename
```bash
multirename --rename --path /home/user/docs/files/ 
```

# Building and Installing multirenamer

## How To Build
I provided a simple CMakeLists.txt file as well as a makefile.

### CMake

1. Create a build directory somewhere (.e.g. ~/build/multirenamer).
2. cd into that directory.
3. Call cmake {path-to-the-source-code}
4. Call make
5. Call make install with root access

### make

1. Create a build directory somewhere (.e.g. ~/build/multirenamer).
2. Copy the source files (.c, .h and the makefile) in to that directory. 
3. cd into that directory.
4. Call make
5. Call make install with root access

After the installation the executable will be installed to /usr/local/bin by default.

# License
The tool is licensed under GPL v2.0, see the file LICENSE for the full license.
