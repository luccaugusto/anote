# anote

Anote is a program for taking notes. This repository provides two implementations, the first one is
a POSIX shell script for taking notes and viewing them in the terminal.
Notes are saved in plain text files in $NOTES_PATH directory or $HOME/.notes if $NOTES_PATH is not set.
run 'anote -h' for help.
The second implementation is a more robust, ncurses based CLI application, still in early development stages.
Probably the CLI application will engulf the shell script in the future.

I recommend adding anote.sh to $PATH and make a 'anote' alias so you don't have to type anote.sh everytime.

# TODO
+ implement remove function alternative without dmenu
