use sdl2::event::Event;
use sdl2::pixels::Color;
use sdl2::rect::Rect;
use sdl2::render::{TextureCreator, WindowCanvas};
use sdl2::video::WindowContext;
use sdl2::Sdl;
use std::env;
use std::path::Path;
use std::time::Duration;
mod settings;

fn render(
    canvas: &mut WindowCanvas,
    texture_creator: &TextureCreator<WindowContext>,
    font: &sdl2::ttf::Font,
) -> Result<(), String> {
    let color = Color::RGB(0, 0, 0);
    canvas.set_draw_color(color);
    canvas.clear();

    // draw greeting
    let hello_text: String = "Hello, SDL2!".to_string();
    let surface = font
        .render(&hello_text)
        .blended(Color::RGBA(255, 0, 0, 255))
        .map_err(|e| e.to_string())?;

    let texture = texture_creator
        .create_texture_from_surface(&surface)
        .map_err(|e| e.to_string())?;

    let target = Rect::new(10, 0, 200, 100);
    canvas.copy(&texture, None, target)?;

    canvas.present();
    Ok(())
}

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
        .window("Cytopia", screen_width, screen_height)
        .position_centered()
        .build()
        .unwrap();

    let mut canvas: WindowCanvas = window.into_canvas().build().unwrap();
    let texture_creator = canvas.texture_creator();

    let ttf_context = sdl2::ttf::init().map_err(|e| e.to_string())?;
    let font_path: &Path = Path::new(&"data/resources/fonts/pixelFJ8pt1.ttf");
    let mut font = ttf_context
        .load_font(font_path, 128)
        .map_err(|e| e.to_string())?;
    font.set_style(sdl2::ttf::FontStyle::BOLD);

    let mut event_pump = context.event_pump().unwrap();
    'running: loop {
        for event in event_pump.poll_iter() {
            match event {
                Event::KeyDown {
                    keycode: Some(keycode),
                    ..
                } => {
                    if keycode == sdl2::keyboard::Keycode::Escape {
                        break 'running;
                    }
                }
                Event::MouseMotion {
                    timestamp,
                    window_id,
                    which,
                    mousestate,
                    x,
                    y,
                    xrel,
                    yrel,
                } => {
                    println!("Mouse moved: x={}, y={}", x, y);
                }
                Event::MouseButtonDown {
                    timestamp,
                    window_id,
                    which,
                    mouse_btn,
                    clicks,
                    x,
                    y,
                } => {
                    println!("Mouse button pressed: x={}, y={}", x, y);
                }
                Event::MouseButtonUp {
                    timestamp,
                    window_id,
                    which,
                    mouse_btn,
                    clicks,
                    x,
                    y,
                } => {
                    println!("Mouse button released: x={}, y={}", x, y);
                }
                Event::Quit { .. } => {
                    break 'running;
                }
                _ => {}
            }
        }
        render(&mut canvas, &texture_creator, &font);

        ::std::thread::sleep(Duration::new(0, 1_000_000_000u32 / 60));
    }
    Ok(())
}
