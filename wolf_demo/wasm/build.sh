#!/bin/sh
em++ wolf.cpp \
-I./../../ \
-I./../../build/wasm32-unknown-emscripten/_deps/boost_leaf-src/include \
-I./../../build/wasm32-unknown-emscripten/_deps/fmt-src/include \
-I./../../build/wasm32-unknown-emscripten/_deps/gsl-src/include \
-L./../../build/wasm32-unknown-emscripten/wolf/ -lwolf \
-DWOLF_STREAM_JANUS \
-o ./www/wolf.js