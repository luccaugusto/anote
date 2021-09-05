use std::env;
use clap::{Arg, App};

pub mod cli;
pub mod fileio;
pub mod constants;
pub mod tags;
pub mod notes;

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

    // General tag is default
    let initial_tag = matches.value_of("tag").unwrap_or(constants::DEFAULT_TAG);

    match matches.occurrences_of("list") {
        1 => {
            interactive = false;
            fileio::list_notes_from_tag(&initial_tag.to_string());
        },
        _ => (),
    };

    if interactive {
        cli::init_cli();
    }
}
