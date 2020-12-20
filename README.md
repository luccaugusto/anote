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
run ```sudo make install```

## Script
The script has 4 functions:
+ Add: adds the note to specified tag. Note is added to General Tag if no tag is specified. Integrates with calcurse.
+ Remove: removes the selected note from the list, if no notes are left on that tag, tag is also removed. CURRENTLY DEPENDS ON DMENU.
+ List: lists notes of specified tag. If no tag was specified, list notes from General Tag.
+ Search: searches for note that matches expression.

Please note that editing a note is not supported on the script version.

Run 'anote.sh -h' for further help.

## Curses application
The curses application is pretty simple, it just shows the notes you have and allow you to manipulate it.
Please note it is still in development, so a few more functionalities are to come (see the TODO list below).
The idea of this program however is to keep things simple, it just takes notes, and takes them well.

## TODO
+ Improve readme
+ Make install
+ Make man page

### Shell
+ implement remove function alternative without dmenu.
+ add function to import every todo item from a project.

### Data
+ Change shell script output format to suit the CLI application as well.
+ save the date a note was added and increase its priority automattically as time passes.

### CLI
+ Scroll only if there are more notes that are not displayed
+ Clock on footer/ Clock mode like nmcnmcppcpcmmcp (whatever, you know what program i'm talking about).
+ Key to expand selected tag if not all its notes are displayed.
+ Possibility to swap main window with side window and show tags panels side by side on the big window
