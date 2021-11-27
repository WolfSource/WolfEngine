fn main() -> core::result::Result<(), Box<dyn std::error::Error>> {
    env!(
        "PKG_CONFIG_PATH",
        "$PKG_CONFIG_PATH enviroment variable was not set"
    );
    Ok(())
}
