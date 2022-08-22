extern crate gtk;

use gtk::prelude::*;
use self::gtk::glib;

use crate::game::Game;

pub struct Menu
{
    screen_width: i32,
    screen_height: i32,

    application: gtk::Application,

    is_running: bool,
    start_game: bool,
    show_options: bool
}

impl Menu
{
    pub fn new(width: i32, height: i32) -> Result<Menu, String>
    {
        let menu_app = gtk::Application::new(Some("com.github.gtk-rs.examples.basic"),
                                          Default::default());

        menu_app.connect_activate(move |menu_app| {Menu::build_ui(menu_app, width, height);});

        let menu = Ok(Menu {
            screen_width: width,
            screen_height: height,

            application: menu_app,

            is_running: true,
            start_game: false,
            show_options: false
        });

        menu
    }

    pub fn build_ui(application: &gtk::Application, width: i32, height: i32) {
        let window = gtk::ApplicationWindow::new(application);

        window.set_title("JTetris");
        window.set_border_width(10);
        window.set_position(gtk::WindowPosition::Center);
        window.set_default_size(width, height);

        let overlay = gtk::Overlay::new();

        // The overlay label.
        let overlay_text = gtk::Label::new(None);
        overlay_text.set_widget_name("overlay-label");
        overlay_text.set_halign(gtk::Align::End);
        overlay_text.set_valign(gtk::Align::Start);

        overlay.add_overlay(&overlay_text);

        let vertical_box = gtk::Box::new(gtk::Orientation::Vertical, 0);

        let margin: i32 = 12;
        let start_button = gtk::Button::builder()
        .label("Start")
        .margin_top(margin)
        .margin_bottom(margin)
        .margin_start(margin)
        .margin_end(margin)
        .build();

        let options_button = gtk::Button::builder()
        .label("Options")
        .margin_top(margin)
        .margin_bottom(margin)
        .margin_start(margin)
        .margin_end(margin)
        .build();

        let exit_button = gtk::Button::builder()
        .label("Exit")
        .margin_top(margin)
        .margin_bottom(margin)
        .margin_start(margin)
        .margin_end(margin)
        .build();

        start_button.connect_clicked(move |_| {Menu::start_game()});
        options_button.connect_clicked(move |_|{Menu::show_options()});
        exit_button.connect_clicked(glib::clone! (@weak application => move |_| Menu::quit(&application)));

        vertical_box.add(&start_button);
        vertical_box.add(&options_button);
        vertical_box.add(&exit_button);

        overlay.add(&vertical_box);
        window.add(&overlay);

        window.show_all();
    }

    pub fn run_loop(&self)
    {
        self.application.run();
    }

    pub fn start_game()
    {
        match Game::new(1024, 768)
        {
            Ok(mut game) => game.run_loop(),
            Err(why) => panic!("{:?}", why),
        };
    }

    pub fn show_options()
    {

    }

    pub fn quit(application: &gtk::Application)
    {
        application.quit();
    }
}
