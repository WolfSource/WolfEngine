if (FALSE)

# corrosion for linking to wolf rust library
FetchContent_Declare(
    corrosion
    GIT_REPOSITORY https://github.com/corrosion-rs/corrosion.git
    GIT_TAG master
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(corrosion)
corrosion_import_crate(MANIFEST_PATH ${CMAKE_CURRENT_SOURCE_DIR}/rust/Cargo.toml)

set_target_properties(
    _cargo-build_wolf_rs
    cargo-build_wolf_rs
    cargo-clean
    cargo-clean_wolf_rs
    cargo-prebuild
    cargo-prebuild_wolf_rs
    PROPERTIES FOLDER "rust") 

endif()