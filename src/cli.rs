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

	terminal.clear()?;

	let (key_tx, rx) = mpsc::channel();
	let tick_rate = Duration::from_millis(200);
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
        terminal.draw(|f| {
            let chunks = Layout::default()
                .direction(Direction::Vertical)
                .margin(1)
                .constraints(
                    [
                    Constraint::Length(config::TABS_SIZE),
                    Constraint::Min(config::NOTES_SIZE),
                    ].as_ref()
                    )
                .split(f.size());
            let block = Block::default()
                .title(taglist[active_tag].get_name().to_string())
                .borders(Borders::TOP);
            f.render_widget(block, chunks[1]);
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
            let tabs = Tabs::new(menu)
                .select(active_tag.into())
                .block(Block::default().title(config::TABS_TITLE).borders(Borders::NONE)) //TODO put borders on config
                .style(Style::default().fg(Color::White)) //TODO put color on config
                .highlight_style(Style::default().fg(Color::Yellow)) //TODO put color on config
                .divider(Span::raw("|"));
            f.render_widget(tabs, chunks[0]);
        })?;

		match rx.recv()? {
			Event::Input(input) => match input.code {
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
