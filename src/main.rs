use std::env;
use clap::{Arg, App};

mod notes;
mod tags;
mod cli;
mod constants;

fn build_file_name(tagname: String) -> String {
    match env::var("NOTES_PATH") {
        Ok(path) => path + &"/anotes" + &tagname,
        Err(_e)  => match env::var("XDG_CONFIG_HOME") {
            Ok(path) => path + &"/anotes" + &tagname,
            Err(_e)  => "~/.config/anote/anotes".to_owned() + &tagname,
        }
    }
}

fn list_notes_from_tag(tagname: String) {
    println!("TODO: LIST NOTES FROM TAG");
}

fn main () {
    let args: Vec<String> = env::args().collect();
    let mut interactive = true;

    let matches = App::new(constants::NAME)
        .version(constants::VERSION)
        .author(constants::AUTHOR)
        .about(constants::ABOUT)
        .arg(Arg::with_name("tag")
                 .short("t")
                 .long("tag")
                 .takes_value(true)
                 .help("tag file name"))
        .arg(Arg::with_name("list")
                 .short("l")
                 .long("list")
                 .takes_value(false)
                 .help("Show help"))
        .get_matches();

    println!("{:?}", matches);

    // General tag is default
    let initial_tag = matches.value_of("tag").unwrap_or(constants::DEFAULT_TAG);

    match matches.occurrences_of("list") {
        1 => {
            interactive = false;
            list_notes_from_tag(initial_tag.to_string());
        },
        _ => (),
    };

    println!("initial tag: {}", initial_tag);

    if interactive {
        cli::init_cli();
    }
}
