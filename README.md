# anote

Anote is a very simple program for taking notes. This repository provides two implementations, the first one is
a POSIX shell script for taking notes and viewing them in the terminal.
Notes are saved in plain text files in $NOTES_PATH directory or $HOME/.notes if $NOTES_PATH is not set.
The second implementation is a more robust, ncurses based CLI application, still in early development stages.
Probably the CLI application will engulf the shell script in the future.

I recommend adding anote.sh to $PATH and make a 'anote' alias so you don't have to type anote.sh everytime.


## Script
The script has 4 functions:
+ Add: adds the note to specified tag. Note is added to General Tag if no tag is specified. Integrates with calcurse.
+ Remove: removes the selected note from the list, if no notes are left on that tag, tag is also removed. CURRENTLY DEPENDS ON DMENU.
+ List: lists notes of specified tag. If no tag was specified, list notes from General Tag.
+ Search: searches for note that matches expression.

Please note that editing a note is not supported on the script version.

Run 'anote.sh -h' for further help.

## Curses application
Currently under development.

## TODO
### Shell
+ implement remove function alternative without dmenu

### Data
+ Implement read/write notes files.
+ Change shell script output format to suit the CLI application as well.

### CLI
+ Implement master slave layout with tag passed on the command line being the inital master and other tags shown on the right side, with a preview of the notes it contains.
+ Tab to change the selected tag window
+ enter to make selected window master
+ tick note as complete
+ vim bindings
+ Implement colors: show more prioritary notes in red, mid priority notes in orange, other notes in white
