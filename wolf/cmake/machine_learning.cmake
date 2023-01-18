if(WOLF_MACHINE_LEARNING_OCR)
    # fetch tesseract
    message("fetching https://github.com/tesseract-ocr/tesseract.git")
    FetchContent_Declare(
        tesseract
        GIT_REPOSITORY https://github.com/tesseract-ocr/tesseract.git
        GIT_TAG main

        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    set(FETCHCONTENT_QUIET OFF)

    set(BUILD_TESTS OFF CACHE BOOL "BUILD_TESTS")
    set(BUILD_TRAINING_TOOLS OFF CACHE BOOL "BUILD_TRAINING_TOOLS")
    set(DISABLE_ARCHIVE ON CACHE BOOL "DISABLE_ARCHIVE")
    set(DISABLE_CURL ON CACHE BOOL "DISABLE_CURL")
    set(FAST_FLOAT ON CACHE BOOL "FAST_FLOAT")
    set(GRAPHICS_DISABLED ON CACHE BOOL "GRAPHICS_DISABLED")
    set(INSTALL_CONFIGS OFF CACHE BOOL "INSTALL_CONFIGS")
    set(SW_BUILD ON CACHE BOOL "SW_BUILD")

    FetchContent_MakeAvailable(tesseract)
    list(APPEND INCLUDES
        ${tesseract_SOURCE_DIR}/include
        ${tesseract_BINARY_DIR}/include
    )

    if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(DEBUG_LIB_EXTENTION "d")
    else()
        set(DEBUG_LIB_EXTENTION "")
    endif()

    message(${CMAKE_BUILD_TYPE}"....DEBUG_LIB_EXTENTION: "${DEBUG_LIB_EXTENTION}"........")

    list(APPEND LIBS
        ${tesseract_BINARY_DIR}/${CMAKE_BUILD_TYPE}/tesseract53${DEBUG_LIB_EXTENTION}.lib
    )

    # fetch opencv
    message("fetching https://github.com/opencv/opencv.git")
    FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.5.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_GetProperties(opencv)

    set(BUILD_LIST core,highgui,videoio CACHE STRING "BUILD_LIST")

    # set(BUILD_opencv_python3 OFF CACHE BOOL "BUILD_opencv_python3")
    set(WITH_IPP OFF CACHE BOOL "WITH_IPP")
    set(BUILD_EXAMPLES OFF CACHE BOOL "BUILD_EXAMPLES")
    set(OPENCV_GENERATE_PKGCONFIG ON CACHE BOOL "OPENCV_GENERATE_PKGCONFIG")
    FetchContent_MakeAvailable(opencv)

    list(APPEND INCLUDES
        ${CMAKE_BINARY_DIR}
        ${opencv_SOURCE_DIR}/include
        ${opencv_SOURCE_DIR}/modules/core/include
        ${opencv_SOURCE_DIR}/modules/highgui/include
        ${opencv_SOURCE_DIR}/modules/imgcodecs/include
        ${opencv_SOURCE_DIR}/modules/imgproc/include
        ${opencv_SOURCE_DIR}/modules/videoio/include
    )

    list(APPEND LIBS
        ${opencv_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_core454${DEBUG_LIB_EXTENTION}.lib
        ${opencv_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_highgui454${DEBUG_LIB_EXTENTION}.lib
        ${opencv_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_imgcodecs454${DEBUG_LIB_EXTENTION}.lib
        ${opencv_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_imgproc454${DEBUG_LIB_EXTENTION}.lib
        ${opencv_BINARY_DIR}/lib/${CMAKE_BUILD_TYPE}/opencv_videoio454${DEBUG_LIB_EXTENTION}.lib
    )

    # fetch rapidjson
    message("fetching  https://github.com/Tencent/rapidjson.git")
    FetchContent_Declare(
        rapidjson
        GIT_REPOSITORY https://github.com/Tencent/rapidjson.git

        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    set(BUILD_TESTING OFF CACHE BOOL "BUILD_TESTING")
    set(RAPIDJSON_BUILD_ASAN OFF CACHE BOOL "RAPIDJSON_BUILD_ASAN")
    set(RAPIDJSON_BUILD_CXX11 OFF CACHE BOOL "RAPIDJSON_BUILD_CXX11")
    set(RAPIDJSON_BUILD_DOC OFF CACHE BOOL "RAPIDJSON_BUILD_DOC")
    set(RAPIDJSON_BUILD_EXAMPLES OFF CACHE BOOL "RAPIDJSON_BUILD_EXAMPLES")
    set(RAPIDJSON_BUILD_TESTS OFF CACHE BOOL "RAPIDJSON_BUILD_TESTS")

    FetchContent_MakeAvailable(rapidjson)

    list(APPEND INCLUDES
        ${rapidjson_SOURCE_DIR}/include
    )

    file(GLOB_RECURSE WOLF_MACHINE_LEARNING_OCR_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/machine_learning/referee_ocr/*"
    )

    list(APPEND SRCS
        ${WOLF_MACHINE_LEARNING_OCR_SRC}
    )
endif()
