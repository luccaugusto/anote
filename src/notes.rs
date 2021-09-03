#[derive(Debug)]
pub struct Note {
    id: u16,
    text: String,
    priority: u8,
}

impl Note {

    pub fn get_id(&self) -> &u16 {
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
}

fn next_id() -> u16 {
    //last_id++
    1
}

pub fn new(text: String, priority: u8) -> Note {
    Note {
        id: next_id(),
        text,
        priority,
    }
}
