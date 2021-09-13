# anote

# IMPORTANT:
Anote is a very simple program for taking notes.
Notes are saved in plain text files in $NOTES_PATH directory or $XDG_CONFIG_HOME/anote/.notes if $NOTES_PATH is not set.

# Instalation
Just put the script somewhere in your PATH.

## Script
The script has 4 functions:
+ Add: adds the note to specified tag. Note is added to General Tag if no tag is specified. Integrates with calcurse.
+ Remove: removes the selected note from the list, if no notes are left on that tag, tag is also removed. CURRENTLY DEPENDS ON DMENU.
+ List: lists notes of specified tag. If no tag was specified, list notes from General Tag.
+ Search: searches for note that matches expression.

Please note that editing a note is not supported on the script version.

Run 'anote.sh -h' for further help.

## TODO

### Shell
+ implement remove function alternative without dmenu.
+ add function to import every todo item from a project.
