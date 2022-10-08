//cargo test test_media_openal --release -- --exact --nocapture
#[cfg(feature = "media_openal")]
#[test]
fn test_media_openal() {
    use std::fs::File;
    use wolf::media::openal::{Format, OpenAl, SampleRate};

    let devices = OpenAl::list_all_devices();
    println!("{:?}", devices);

    let current_dir = std::env::current_dir().unwrap();
    let wolf_dir = current_dir.join("wolf");
    let wav_dir = if wolf_dir.exists() {
        wolf_dir.join("sine.wav")
    } else {
        current_dir.join("sine.wav")
    };

    let mut wav_file = File::open(wav_dir).unwrap();
    let (_header, data) = wav::read(&mut wav_file).unwrap();

    //println!("wav header:{:?} data:{:?}", header, data);

    let channels = std::num::NonZeroU32::new(2).unwrap();
    let openal = OpenAl::new(Format::Stereo16, SampleRate::_44100, 25, channels).unwrap();
    openal.open().unwrap();

    let data = data.as_sixteen().unwrap();
    openal.update_from_heap_i16(data).unwrap();

    openal.close(false);
}
