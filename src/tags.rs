use std::{sync::atomic::{AtomicUsize, Ordering}};
use crate::notes::Note;

#[derive(Debug)]
pub struct Tag {
    id: usize,
    name: String,
    note_list: Vec<Note>,
}

impl Tag {

    pub fn get_name(&self) -> &String {
        &self.name
    }

    pub fn get_id(&self) -> &usize {
        &self.id
    }

    pub fn get_note_list(&self) -> &Vec<Note> {
        &self.note_list
    }

    pub fn add_note(&mut self, note: Note) {
        self.note_list.push(note);
    }

    pub fn set_name(&mut self, name: String) {
        self.name = name;
    }

    pub fn del_note(&mut self, note_id: usize) {
        if let Some(pos) = self.note_list.iter().position(|x| *x.get_id() == note_id) {
            self.note_list.remove(pos);
        }
    }

    pub fn new(name: String) -> Tag {
        Tag {
            id: TAG_COUNTER.fetch_add(1, Ordering::SeqCst),
            name,
            note_list: Vec::new(),
        }
    }
}

static TAG_COUNTER: AtomicUsize = AtomicUsize::new(0);
