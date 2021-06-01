mod game;
mod tetromino;
mod menu;

fn main()
{
    let mut application = menu::Menu::new(1024, 768).unwrap();
    application.run_loop();
}
