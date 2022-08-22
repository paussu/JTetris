mod game;
mod tetromino;
mod menu;

fn main()
{
    match menu::Menu::new(400, 400)
    {
        Ok(application) => application.run_loop(),
        Err(why) => panic!("{:?}", why),
    };
}
