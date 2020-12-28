# anote

Anote is a very simple program for taking notes. This repository provides two implementations, the first one is
a POSIX shell script for taking notes and viewing them in the terminal.
The second implementation is a more robust, ncurses based CLI application, still in development.
Probably the CLI application will engulf the shell script in the future.
Notes are saved in plain text files in $NOTES_PATH directory or $XDG_CONFIG_HOME/anote/.notes if $NOTES_PATH is not set.

This program follows the suckless philosophy, the configuration file is the source code itself.
To change a configuration you must first change it on src/config.def.h and run ```make config```.

This program is licensed under the GNU GPL3 License.

# Instalation
run ```make && sudo make install```

## Script
The script has 4 functions:
+ Add: adds the note to specified tag. Note is added to General Tag if no tag is specified. Integrates with calcurse.
+ Remove: removes the selected note from the list, if no notes are left on that tag, tag is also removed. CURRENTLY DEPENDS ON DMENU.
+ List: lists notes of specified tag. If no tag was specified, list notes from General Tag.
+ Search: searches for note that matches expression.

Please note that editing a note is not supported on the script version.

Run 'anote.sh -h' for further help.

## Curses application
The curses application is pretty simple, it just shows the notes you have and allow you to manipulate them.
The idea of this program however is to keep things simple, it just takes notes.
There might be some bugs on some specific cases, i decided to move on to another project instead of hunting them down,
feel free to fix them or implement some of the TODO items.

## TODO

### Shell
+ implement remove function alternative without dmenu.
+ add function to import every todo item from a project.

### Data
+ Change shell script output format to suit the CLI application as well.
+ save the date a note was added and increase its priority automattically as time passes.

### CLI
+ Do not use a menu for listing notes on the main window (it's going to fix many bugs related to long notes).
+ scroll commands on footer if they dont fit
+ Possibility to swap main window with side window and show tags panels side by side on the big window
+ noninteractive remove function
+ add function without the -a flag
