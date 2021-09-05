use std::env;
use std::fs::File;
use std::io::{BufRead, BufReader};

use crate::constants;
use crate::tags::Tag;

fn build_file_name() -> String {
    match env::var("NOTES_PATH") {
        Ok(path) => path + &"/" + constants::FILENAME,
        Err(_e)  => match env::var("XDG_CONFIG_HOME") {
            Ok(path) => path + &"/" + constants::FILENAME,
            Err(_e)  => constants::DEFAULT_PATH.to_owned() + &"/" + constants::FILENAME,
        }
    }
}

pub fn list_notes_from_tag(tagname: &String) {
    let filename = build_file_name();
    let mut cur_line: Vec<&str>;

    let mut file = match File::open(&filename) {
        Err(e) => panic!("couldn't open {}: {}", filename, e),
        Ok(file) => file,
    };

    let reader = BufReader::new(file);

    for line in reader.lines() {
        let cur_line = line.as_ref().unwrap()
            .split(constants::SEPARATOR)
            .collect::<Vec<&str>>();

        if cur_line[0].eq(tagname) {
            match line {
                Ok(l) => println!("{}", l),
                Err(e) => println!("{}", e),
            };
        }
    }
}

pub fn load_notes_from_file(taglist: &Vec<Tag>,filename: &String) {
    ()
}
