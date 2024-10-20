use sdl2::Sdl;
mod settings; // Declare the module
use std::env;
use sdl2::pixels::Color;
use sdl2::event::Event;

fn main() -> Result<(), String> {
    let context: Sdl = sdl2::init().unwrap();
    let video_subsystem = context.video().unwrap();

    let settings_path = env::current_dir()
        .unwrap()
        .join("data/resources/settings.json");
    let settings = settings::Settings::load_from_file(
        settings_path.to_str().ok_or("Invalid path")?, // More informative error
    )
    .unwrap();
    let screen_width = settings.graphics.get_width();
    let screen_height = settings.graphics.get_height();

    let window = video_subsystem
        .window(
            "SDL2",
            screen_width,
            screen_height,
        )
        .build()
        .unwrap();

    let mut canvas = window.into_canvas().build().unwrap();

    let screen_area = sdl2::rect::Rect::new(0, 0, screen_width, screen_height);
    let clear_color = Color::RGB(64, 192, 255);

    let mut running = true;
    while running {
        canvas.set_draw_color(clear_color);
        canvas.fill_rect(screen_area).unwrap();
        canvas.present();
        for event in context.event_pump().unwrap().poll_iter() {
            match event {
                Event::KeyDown {
                    keycode: Some(keycode),
                    ..
                } => {
                    if keycode == sdl2::keyboard::Keycode::Escape {
                        running = false;
                    }
                },
                Event::MouseMotion { timestamp, window_id, which, mousestate, x, y, xrel, yrel } => {
                    println!("Mouse moved: x={}, y={}", x, y);
                },
                Event::Quit { .. } => {
                    running = false;
                }
                _ => {}
            }
        }
    }
    Ok(())
}
