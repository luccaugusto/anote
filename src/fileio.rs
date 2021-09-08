use std::env;
use std::fs::File;
use std::path::Path;
use std::num::ParseIntError;
use std::io::{BufRead, BufReader, Error, Write};

use crate::config;
use crate::tags::Tag;
use crate::notes::Note;

fn build_file_name() -> String {
    match env::var("NOTES_PATH") {
        Ok(path) => path + &"/" + config::FILENAME,
        Err(_e)  => match env::var("XDG_CONFIG_HOME") {
            Ok(path) => path + &"/" + config::FILENAME,
            Err(_e)  => config::DEFAULT_PATH.to_owned() + &"/" + config::FILENAME,
        }
    }
}

pub fn list_notes_from_tag(tagname: &String) {
    let filename = build_file_name();
    let path = Path::new(&filename);

    let file = match File::open(&filename) {
        Err(e) => panic!("couldn't open {}: {}", path.display(), e),
        Ok(file) => file,
    };

    let reader = BufReader::new(file);

    for line in reader.lines() {
        let cur_line = line.as_ref().unwrap()
            .split(config::SEPARATOR)
            .collect::<Vec<&str>>();

        if cur_line[0].eq(tagname) {
            match line {
                Ok(l) => println!("{}", l),
                Err(e) => println!("{}", e),
            };
        }
    }
}

pub fn load_notes_from_file(taglist: &mut Vec<Tag>) -> Result<(), ParseIntError> {
    let filename = build_file_name();
    let path = Path::new(&filename);

    let file = match File::open(&path) {
        Err(e) => panic!("couldn't open {}: {}", path.display(), e),
        Ok(file) => file,
    };

    let reader = BufReader::new(file);

    //File is a csv in format: tag_name,priority,note_text
    for line in reader.lines() {
        let cur_line = line.as_ref().unwrap()
            .split(config::SEPARATOR)
            .collect::<Vec<&str>>();

        let tag_name = cur_line[0];

        let priority = match cur_line[1].parse::<u8>() {
            Ok(number)  => number,
            Err(e) => return Err(e),
        };

        let note_text = cur_line[2];

        let note = Note::new(note_text.to_owned(), priority);

        //Tag exists in list
        if let Some(tag_pos) = taglist.iter().position(|x| *x.get_name() == tag_name) {
            taglist[tag_pos].add_note(note);
        }

        //Create new tag and add it to list
        else {
            let mut new_tag = Tag::new(tag_name.to_string());
            new_tag.add_note(note);
            taglist.push(new_tag);
        }
    }

    Ok(())
}

pub fn write_notes_to_file(taglist: &mut Vec<Tag>) -> Result<(), Error> {

    let filename = build_file_name();
    let path = Path::new(&filename);

    let mut file = match File::create(&path) {
        Err(e) => return Err(e),
        Ok(file) => file,
    };

    // writes to notesfile in format
    // tagname,priority,note text
    for tag in taglist.iter() {
        for note in tag.get_note_list() {
            let note_string = format!("{},{},{}\n",
                   tag.get_name(),
                   note.get_priority(),
                   note.get_text()
            );
            file.write_all(note_string.as_bytes()).expect("File not Written");
        }
    }

    Ok(())
}
