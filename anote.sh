#!/bin/sh

#-Simple script for taking notes in the terminal.
#-Notes are saved in $NOTES_PATH directory or $HOME/.notes if $NOTES_PATH is not set
#-run 'anote -h' for help

#reset for getopts
OPTIND=1

[ $NOTES_PATH ] ||  NOTES_PATH="$HOME/.notes/"
# if missing trailing /, add it
[ "${NOTES_PATH: -1}" == '/' ] || NOTES_PATH="$NOTES_PATH/"
GENERAL='.notesgeneral'
WHICH_NOTES=''
DEFAULT=false
OPERATION='a'
TAG='general'
NOTE=''

add_note()
{
	if ! [ "$NOTE" = '' ]; then
		if [ ! -f $NOTES_PATH.notes$TAG ]; then
			touch $NOTES_PATH.notes$TAG
		fi
		#gets the last note number from file
		read NOTE_NUMBER<<<$(awk 'END{print $1}' $NOTES_PATH.notes$TAG | sed s/~//g)
		NOTE_NUMBER=$((NOTE_NUMBER+1))
		DATE=$(date +%Y-%m-%d)
		echo "~$NOTE_NUMBER	$DATE	'$NOTE'">>$NOTES_PATH.notes$TAG
	else
		echo "No empty notes"
	fi
}

list_notes()
{
	if [ $1 ]; then
		if [ $1 == -d ]; then
			WHICH_NOTES=".notesgeral";shift
		else
			WHICH_NOTES=".notes$1"; shift
		fi
	else
		WHICH_NOTES=.notes$(ls -a $NOTES_PATH | grep ^.notes | awk -F " " '{print substr($1,7)}'| dmenu -l 10)
	fi

	if test -f $NOTES_PATH$WHICH_NOTES; then
		awk -F "\t" '{print $1". " $3}' $NOTES_PATH$WHICH_NOTES
	else
		echo 'No Notes'
	fi
}

filter_notes()
{
	if [ $2 ]; then
		if [ $2 == -d ]; then
			WHICH_NOTES=".notesgeral";shift
		else
			WHICH_NOTES=".notes$2"; shift
			if test -f $NOTES_PATH$WHICH_NOTES; then
				cat $NOTES_PATH$WHICH_NOTES | grep "$1" | awk -F "\t" '{print $1. "\033[34m "$3"\033[0m"}'
				shift
			else
				echo 'No Notes'
			fi
		fi
	else
		for list in $(ls -a $NOTES_PATH | grep .notes); do
			cat $NOTES_PATH$WHICH_NOTES | grep "$1" | awk -F "\t" '{print $1. "\033[34m "$3"\033[0m"}'
			shift
		done
	fi

}

show_help()
{
	echo 'aNote [SOME NOTE] [-t] [SOME TAG]: Adds a note to a specific Tag. If no tag is informed adds to general notes'
	echo 'aNote -l [SOME TAG]:Prompts with a dmenu to select notes file to list notes from
					-d flag to list general notes'
	echo 'aNote -f [WORD/DATE]: Search for notes with that word, tag or date'
	echo 'aNote -r [TAG]: prompts with a dmenu to select note to delete. if no tag is informed dmenu prompts with tag and then 			notes'
}

remove_note()
{
	if [ $1 ]; then
		if [ $1 == -d ]; then
			WHICH_NOTES=".notesgeral";shift
		else
			WHICH_NOTES=".notes$1"; shift
		fi
	else
		WHICH_NOTES=.notes$(ls -a $NOTES_PATH | grep ^.notes | awk -F " " '{print substr($1,7)}'| dmenu -l 10)
	fi

	if test -f "$NOTES_PATH$WHICH_NOTES"; then
		NOTE=$(cat $NOTES_PATH$WHICH_NOTES | sed "s/	/ /g" | dmenu -l 10 | awk -F " " '{print $1}')
		if [ "$NOTE" ];then
			sed -i '/'"$NOTE"'/d' $NOTES_PATH$WHICH_NOTES
			ALL_NOTES=$(cat $NOTES_PATH$WHICH_NOTES)
			if [ ! "$ALL_NOTES" ]; then
				rm $NOTES_PATH$WHICH_NOTES
			fi
		fi
	else
		echo 'No Notes'
	fi
}


while getopts ":df:hlrt:" opt; do
	#parse arguments before calling any function
	case $opt in
		-d) DEFAULT=true
			;;
		-f) shift; filter_notes "$1" "$2"
			;;
		-h) show_help
			exit 0
			;;
		-l) OPERATION='l'
			;;
		-r) OPERATION='r'
			;;
		-t) TAG=$OPTARG
			;;
		\?) echo "invalid option: -$OPTARG" >&2
			show_help
			;;
	esac
done

shift $((OPTIND -1))
# leftover arg is note istself
NOTE=${@[0]}

case $OPERATION in
	'a') add_note $NOTE $TAG
		;;
	'l') list_notes
		;;
	'f') filter_notes
		;;
	'r') remove_note
		;;
esac

