use std::io;
use tui::Terminal;
//use termion::raw::IntoRawMode;
//use tui::backend::TermionBackend;
use tui::backend::CrosstermBackend;
use crossterm::{
    event::{self, Event as CEvent, KeyCode},
    terminal::{disable_raw_mode, enable_raw_mode},
};
use tui::widgets::{Widget, Block, Borders};
use tui::layout::{Layout, Constraint, Direction};

use crate::tags::Tag;

enum Event<I> {
    Input(I),
    Tick,
}

fn init_cli(initial_tag: &str) -> Result<(), io::Error> {
    //Termion Backend
    //let stdout = io::stdout().into_raw_mode()?;
    //let backend = TermionBackend::new(stdout);

    //Crossterm Backend
    enable_raw_mode().expect("can run in raw mode");
    let stdout = io::stdout();
    let backend = CrosstermBackend::new(stdout);

    let mut terminal = Terminal::new(backend)?;
    terminal.draw(|f| {
        let chunks = Layout::default()
            .direction(Direction::Vertical)
            .margin(1)
            .constraints(
                [
                Constraint::Percentage(10),
                Constraint::Percentage(90),
                ].as_ref()
                )
            .split(f.size());
        let block = Block::default()
            .title("Tags")
            .borders(Borders::NONE);
        f.render_widget(block, chunks[0]);
        let block = Block::default()
            .title(initial_tag.to_string())
            .borders(Borders::TOP);
        f.render_widget(block, chunks[1]);
    })
}

//fn get_key() {
//    match rx.recv()? {
//        Event::Input(event) => match event.code {
//            KeyCode::Char('q') => {
//                disable_raw_mode()?;
//                terminal.show_cursor()?;
//                break;
//            }
//            KeyCode::Char('h') => active_menu_item = MenuItem::Home,
//            KeyCode::Char('p') => active_menu_item = MenuItem::Pets,
//            KeyCode::Char('a') => {
//                add_random_pet_to_db().expect("can add new random pet");
//            }
//            KeyCode::Char('d') => {
//                remove_pet_at_index(&mut pet_list_state).expect("can remove pet");
//            }
//            KeyCode::Down => {
//                if let Some(selected) = pet_list_state.selected() {
//                    let amount_pets = read_db().expect("can fetch pet list").len();
//                    if selected >= amount_pets - 1 {
//                        pet_list_state.select(Some(0));
//                    } else {
//                        pet_list_state.select(Some(selected + 1));
//                    }
//                }
//            }
//            KeyCode::Up => {
//                if let Some(selected) = pet_list_state.selected() {
//                    let amount_pets = read_db().expect("can fetch pet list").len();
//                    if selected > 0 {
//                        pet_list_state.select(Some(selected - 1));
//                    } else {
//                        pet_list_state.select(Some(amount_pets - 1));
//                    }
//                }
//            }
//            _ => {}
//        },
//        Event::Tick => {}
//    }
//
//}

//fn execution_loop(taglist: &mut Vec<Tag>) {
//    let (tx, rx) = mpsc::channel();
//    let tick_rate = Duration::from_millis(200);
//    thread::spawn(move || {
//        let mut last_tick = Instant::now();
//        loop {
//            let timeout = tick_rate
//                .checked_sub(last_tick.elapsed())
//                .unwrap_or_else(|| Duration::from_secs(0));
//
//            if event::poll(timeout).expect("poll works") {
//                if let CEvent::Key(key) = event::read().expect("can read events") {
//                    tx.send(Event::Input(key)).expect("can send events");
//                }
//            }
//
//            if last_tick.elapsed() >= tick_rate {
//                if let Ok(_) = tx.send(Event::Tick) {
//                    last_tick = Instant::now();
//                }
//            }
//        }
//    });
//    loop {
//        //draw_cli();
//        //get_key();
//        break;
//    }
//}

fn housekeeping() {
    disable_raw_mode().expect("Should be running in raw mode");
}

pub fn init_anote(taglist: &mut Vec<Tag>, initial_tag: &str) -> Result<(), io::Error>  {
    init_cli(initial_tag).expect("CLI could not be run");

    //create_tabs(&taglist);
    //execution_loop(&taglist);

    housekeeping();
    Ok(())
}
