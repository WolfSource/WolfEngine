if not exist "librist/" (
    echo fetching rist
    git clone --recursive https://code.videolan.org/rist/librist.git
)
cd librist
meson setup build --backend vs2022 --default-library static --buildtype %1
meson compile -C build