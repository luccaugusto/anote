use std::io;
use clap::{Arg, App};

pub mod cli;
pub mod tags;
pub mod notes;
pub mod fileio;
pub mod config;
pub mod constants;

fn main () -> Result<(), io::Error>{
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

    // General tag is default
    let initial_tag_name = matches.value_of("tag").unwrap_or(config::DEFAULT_TAG);

    match matches.occurrences_of("list") {
        1 => {
            interactive = false;
            fileio::list_notes_from_tag(&initial_tag_name.to_string());
        },
        _ => (),
    };

    if interactive {
        let mut taglist: Vec<tags::Tag> = Vec::new();
        let mut initial_tag_pos: usize;

        fileio::load_notes_from_file(&mut taglist).expect("Error: file could not be read");

        initial_tag_pos = 0;
        if let Some(pos) = taglist.iter().position(|t| t.get_name() == initial_tag_name) {
            initial_tag_pos = pos;
        }

        cli::run_anote(&mut taglist, initial_tag_pos).expect("Error");
        fileio::write_notes_to_file(&mut taglist).expect("Error: File not written");
    };

    Ok(())
}
