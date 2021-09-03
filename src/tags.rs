use crate::notes::Note;

#[derive(Debug)]
pub struct Tag {
    id: u16,
    name: String,
    note_list: Vec<Note>,
}

impl Tag {

    pub fn get_name(&self) -> &String {
        &self.name
    }

    pub fn get_id(&self) -> &u16 {
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

    pub fn del_note(&mut self, note_id: u16) {
        //TODO
        //self.note_list.remove(self.note_list.iter().position(|x| x.get_id() == note_id));
        let a = note_id;
    }
}

fn next_id() -> u16 {
    //last_id++
    1
}

pub fn build_tag(name: String) -> Tag {
    Tag {
        id: next_id(),
        name,
        note_list: Vec::new(),
    }
}
