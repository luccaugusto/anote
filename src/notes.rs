use std::{sync::atomic::{AtomicUsize, Ordering}};

#[derive(Debug)]
pub struct Note {
    id: usize,
    text: String,
    priority: u8,
}

impl Note {

    pub fn get_id(&self) -> &usize {
        &self.id
    }

    pub fn get_text(&self) -> &String {
        &self.text
    }

    pub fn get_priority(&self) -> &u8 {
        &self.priority
    }

    pub fn set_text(&mut self, text: String) {
        self.text = text;
    }

    pub fn set_priority(&mut self, priority: u8) {
        self.priority = priority;
    }

    pub fn new(text: String, priority: u8) -> Note {
        Note {
            id: NOTE_COUNTER.fetch_add(1, Ordering::SeqCst),
            text,
            priority,
        }
    }
}

static NOTE_COUNTER: AtomicUsize = AtomicUsize::new(0);
