use std::io::{self, Write};
//use termion::raw::IntoRawMode;
use tui::{
	//backend::TermionBackend;
	backend::CrosstermBackend,
	layout::{Layout, Constraint, Direction, Margin},
	style::{Color, Modifier, Style},
	text::{Span, Spans},
	widgets::{
		Block, BorderType, Borders, List, ListItem, ListState, Paragraph, Row, Table, Tabs, Widget
	},
	Terminal,
};
use crossterm::event::{KeyEvent, KeyModifiers};
use crossterm::{
	event::{self, DisableMouseCapture, EnableMouseCapture, Event as CEvent, KeyCode},
	execute,
	terminal::{disable_raw_mode, enable_raw_mode, EnterAlternateScreen, LeaveAlternateScreen},
};
use std::sync::mpsc;
use std::time::{Duration, Instant};
use std::thread;

use crate::notes::Note;
use crate::tags::Tag;
use crate::config;

#[derive(Debug)]
enum Event {
	Input(KeyEvent),
	Tick,
}

fn build_tag_notes_list<'a>(tag: &Tag) -> List<'a> {
	let notes_block = Block::default()
		.borders(Borders::TOP) //TODO put border on config
		.style(Style::default().fg(Color::White))
		.title("-------".to_owned() + tag.get_name() + " notes:") //TODO put title on config
		.border_type(BorderType::Rounded); //TODO put border style on config

	let note_list = tag.get_note_list();

	let items: Vec<_> = note_list
		.iter()
		.map(|note| {
			ListItem::new(Spans::from(
                vec![
                    Span::styled(
				        "    ".to_owned() + note.get_text(),
				        Style::default(),
			        )
                ]
            ))
		})
		.collect();

	List::new(items)
        .block(notes_block)
        .highlight_style(
		    Style::default()
			.bg(Color::Yellow) //TODO put color on config
			.fg(Color::Black) //TODO put color on config
			.add_modifier(Modifier::BOLD), //TODO put modifier on config
	    )
}

fn build_tags_menu(taglist: &Vec<Tag>, active_tag: usize) -> Tabs {
	let menu = taglist
		.iter()
		.map(|t| {
			Spans::from(vec![
			Span::styled(
				t.get_name(),
				Style::default().fg(Color::White)//TODO put color on config
				),
			])
		})
	.collect();
	Tabs::new(menu)
		.select(active_tag.into())
		.block(Block::default().title("-------".to_owned() + config::TABS_TITLE).borders(Borders::NONE)) //TODO put borders on config
		.style(Style::default().fg(Color::White)) //TODO put color on config
		.highlight_style(Style::default().fg(Color::Yellow)) //TODO put color on config
		.divider(Span::raw("|")) //TODO put divider on config
}

fn configure_screen() -> Layout {
    Layout::default()
        .direction(Direction::Vertical)
        .margin(1) //TODO put margins on config
        .constraints(
            [
                Constraint::Length(config::TABS_SIZE),
                Constraint::Min(config::NOTES_SIZE),
            ].as_ref()
        )
}

fn build_note_details_block(note: &Note) -> (Block, Layout) {
    let (mut block, mut layout) = build_prompt_block("Note Details");
    (block, layout)
}

fn build_prompt_block(title: &str) -> (Block, Layout) {
    let layout = Layout::default()
        .direction(Direction::Horizontal)
        .constraints(
            [
                Constraint::Percentage(50), //TODO put prompt size on config
            ].as_ref()
        )
        .margin(1);//TODO: put margin on config

    let block = Block::default()
        .title(title.to_owned())
        .borders(Borders::ALL)
        .border_type(BorderType::Rounded)
        .style(Style::default().bg(Color::Black));

    (block, layout)
}

pub fn run_anote(taglist: &mut Vec<Tag>, mut active_tag: usize) -> Result<(), Box<dyn std::error::Error>>  {

	//Termion Backend
	//let stdout = io::stdout().into_raw_mode()?;
	//let backend = TermionBackend::new(stdout);

	//Crossterm Backend
	enable_raw_mode()?;

	let mut stdout = io::stdout();
	execute!(stdout, EnterAlternateScreen, EnableMouseCapture)?;
	let backend = CrosstermBackend::new(stdout);

	let mut terminal = Terminal::new(backend)?;
    let chunks = configure_screen();
    let mut should_prompt = false;

	let (key_tx, rx) = mpsc::channel();
	let tick_rate = Duration::from_millis(200);

	let mut note_list_state = ListState::default();
	note_list_state.select(Some(0));

	terminal.clear()?;

	thread::spawn(move || {
		let mut last_tick = Instant::now();
		loop {
			let timeout = tick_rate
				.checked_sub(last_tick.elapsed())
				.unwrap_or_else(|| Duration::from_secs(0));

			if event::poll(timeout).expect("poll works") {
				if let CEvent::Key(key) = event::read().expect("can read events") {
					key_tx.send(Event::Input(key)).expect("can send events");
				}
			}

			if last_tick.elapsed() >= tick_rate {
				if let Ok(_) = key_tx.send(Event::Tick) {
					last_tick = Instant::now();
				}
			}
		}
	});

	loop {
		terminal.draw(|screen| {
            let t_chunks = chunks.split(screen.size());

			let notes_list = build_tag_notes_list(&taglist[active_tag]);

            //render widgets
			screen.render_widget(build_tags_menu(&taglist, active_tag), t_chunks[0]);
			screen.render_stateful_widget(notes_list, t_chunks[1], &mut note_list_state);
		})?;

		match rx.recv()? {
			Event::Input(input) => match input.code {
                KeyCode::Char('i') => {
                    let mut input = String::new();
                    io::stdin().read_line(&mut input)
                        .expect("Can't read from stdin");

                    taglist[active_tag].add_note(
                        Note::new(input, 0)
                        );
                }
                KeyCode::Char('d') | KeyCode::Delete => {
                    if let Some(selected) = note_list_state.selected() {
                        let note_id = taglist[active_tag].get_note_list()[selected].get_id().to_owned();
                        taglist[active_tag].del_note(note_id);

                        note_list_state.select(Some(0));

                        //Remove noteless tags
                        if taglist[active_tag].get_note_list().len() == 0 && taglist.len() > 1 {
                            taglist.remove(active_tag);
                            active_tag = 0;
                        }
                    }
                }
				KeyCode::Char('j') | KeyCode::Down => {
					if let Some(selected) = note_list_state.selected() {
                        // as length in a usize we can't subtract below 0 so
                        // to avoid overflow, always sum by len-1 so it simulates stopping one
                        // before on the circular list
                        let length = &taglist[active_tag].get_note_list().len();
                        if *length > 0 {
                            let next = (selected + length - 1) % length;
                            note_list_state.select(Some(next));
                        }
					}
				}
				KeyCode::Char('k') | KeyCode::Up => {
					if let Some(selected) = note_list_state.selected() {
                        if taglist[active_tag].get_note_list().len() > 0 {
                            let next = (selected + 1) % &taglist[active_tag].get_note_list().len();
                            note_list_state.select(Some(next));
                        }
					}
				}
                KeyCode::Char('l') | KeyCode::Enter => {
                    //should_prompt = true;
                    if let Some(selected) = note_list_state.selected() {
                        terminal.draw(|screen| {
                            let (block, layout) = build_note_details_block(&taglist[active_tag].get_note_list()[selected]);
			                screen.render_widget(block, layout.split(screen.size())[0]);
                            }
                        )?;
                    }
                }
				KeyCode::Tab => {
					active_tag = (active_tag + 1) % taglist.len();
                    note_list_state.select(Some(0));
				}
				KeyCode::Char('q') | KeyCode::Esc => {
					break;
				}
				KeyCode::Char('c') if input.modifiers == KeyModifiers::CONTROL => {
					break;
				}
				_ => {}
			},
			Event::Tick => {}
		}
	}

	//HOUSEKEEPING
	disable_raw_mode()?;
	execute!(
		terminal.backend_mut(),
		LeaveAlternateScreen,
		DisableMouseCapture
		)?;
	terminal.show_cursor()?;

	Ok(())
}
