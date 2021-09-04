mod notes;
mod tags;
mod cli;

fn main () {
    let mut general: tags::Tag = tags::Tag::new(String::from("general"));
    let note: notes::Note = notes::Note::new(String::from("teste"), 2);
    let note2: notes::Note = notes::Note::new(String::from("teste2"), 3);

    println!("{:?}", general);
    println!("{:?}", note);

    general.add_note(note);
    general.add_note(note2);

    let notelist=general.get_note_list();

    println!("{:?}", general.get_id());
    println!("{:?}", general.get_name());
    println!("{:?}", notelist);
    println!("{:?}", notelist[0].get_text());

    general.del_note(1);
    println!("{:?}", general);
}
