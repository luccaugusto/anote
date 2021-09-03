mod notes;
mod tags;

fn main () {
    let mut general: tags::Tag = tags::Tag::new(String::from("general"));
    let note: notes::Note = notes::Note::new(String::from("teste"), 2);

    general.add_note(note);
    println!("{:?}", general);

    general.del_note(1);
    println!("{:?}", general);

}
