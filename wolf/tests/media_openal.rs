//cargo test test_media_openal --release -- --exact --nocapture
#[cfg(feature = "media_openal")]
#[test]
fn test_media_openal() {
    use std::fs::File;
    use wolf::media::openal::{Format, OpenAl, SampleRate};

    let devices = OpenAl::list_all_devices();
    println!("{devices:?}");

    let current_dir = std::env::current_dir().unwrap();
    let mut path = current_dir.join("content/audio/sine.wav");
    if !path.exists() {
        path = current_dir.join("../content/audio/sine.wav");
    }

    let mut wav_file = File::open(path).unwrap();
    let (_header, data) = wav::read(&mut wav_file).unwrap();

    let channels = std::num::NonZeroU32::new(2).unwrap();
    let openal = OpenAl::new(Format::Stereo16, SampleRate::_44100, 25, channels).unwrap();
    openal.open().unwrap();
    openal.reset();

    let data = data.as_sixteen().unwrap();
    openal.update_from_heap_i16(data).unwrap();

    openal.close(false);
}
