use serde::{Deserialize, Serialize};
use once_cell::sync::Lazy;
use std::fs;

#[derive(Serialize, Deserialize, Debug)]
pub struct Settings {
    pub settings_version: u32,
    pub audio: Audio,
    pub config_files: ConfigFiles,
    pub game: Game,
    pub graphics: Graphics,
    pub user_interface: UserInterface,
    pub debug: Debug,
}

#[derive(Serialize, Deserialize, Debug)]
struct Audio {
    audio_3d_status: bool,
    audio_channels: u32,
    music_volume: f32,
    play_music: bool,
    play_sound_effects: bool,
    sound_effects_volume: u32,
}

#[derive(Serialize, Deserialize, Debug)]
struct ConfigFiles {
    audio_config_json_file: String,
    tile_data_json_file: String,
    ui_data_json_file: String,
    ui_layout_json_file: String,
}

#[derive(Serialize, Deserialize, Debug)]
struct Game {
    biome: String,
    language: String,
    map_size: u32,
    max_elevation_height: u32,
    show_buildings_in_blueprint: bool,
    zone_layer_transparency: f32,
}

#[derive(Serialize, Deserialize, Debug)]
pub struct Graphics {
    full_screen: bool,
    full_screen_mode: u32,
    resolution: Resolution,
    vsync: bool,
}

#[derive(Serialize, Deserialize, Debug)]
struct Resolution {
    screen_height: u32,
    screen_width: u32,
}

#[derive(Serialize, Deserialize, Debug)]
struct UserInterface {
    build_menu_position: String,
    font_filename: String,
    language: String,
    sub_menu_button_height: u32,
    sub_menu_button_width: u32,
    default_font_size: u32,
}

#[derive(Serialize, Deserialize, Debug)]
struct Debug {
    write_error_log_to_file: bool,
}

// a function to get width and height of the resolution
impl Graphics {
    pub fn get_width(&self) -> u32 {
        self.resolution.screen_width
    }

    pub fn get_height(&self) -> u32 {
        self.resolution.screen_height
    }
}

impl Settings {
  pub fn load_from_file(file_path: &str) -> Result<Self, Box<dyn std::error::Error>> {
      let file_contents = fs::read_to_string(file_path)?;
      let object: Settings = serde_json::from_str(&file_contents)?;
      Ok(object)
  }
}
