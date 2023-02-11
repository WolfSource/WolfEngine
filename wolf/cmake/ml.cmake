if(WOLF_ML_OCR)

    if(LINUX)
        # fetch leptonica
        message("fetching https://github.com/DanBloomberg/leptonica.git")
        FetchContent_Declare(
            leptonica
            GIT_REPOSITORY https://github.com/DanBloomberg/leptonica.git
            GIT_TAG 1.80.0
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        FetchContent_Populate(leptonica)

        add_custom_command(OUTPUT lept_config.out COMMAND cmake -B ${leptonica_BINARY_DIR} -DBUILD_SHARED_LIBS=1 -DCMAKE_INSTALL_PREFIX:PATH=${leptonica_BINARY_DIR}/install ${leptonica_SOURCE_DIR} )
        add_custom_target(lept_config ALL DEPENDS lept_config.out)
        add_custom_command(OUTPUT lept_build.out COMMAND cmake --build ${leptonica_BINARY_DIR} --target install )
        add_custom_target(lept_build ALL DEPENDS lept_build.out)
    endif()

    # fetch tesseract
    message("fetching https://github.com/tesseract-ocr/tesseract.git")
    FetchContent_Declare(
        tesseract
        GIT_REPOSITORY https://github.com/tesseract-ocr/tesseract.git
        GIT_TAG main

        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    if(WIN64)
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

        link_directories(${tesseract_BINARY_DIR}/${CMAKE_BUILD_TYPE})

        list(APPEND LIBS
            tesseract
        )
    elseif(LINUX)
        FetchContent_Populate(tesseract)

        list(APPEND INCLUDES
        ${tesseract_SOURCE_DIR}/include
        ${tesseract_BINARY_DIR}/install/include
        )

        link_directories(${tesseract_BINARY_DIR}/install/lib)
        list(APPEND LIBS
            tesseract
        )

        add_custom_command(OUTPUT tess_config.out COMMAND cmake -B ${tesseract_BINARY_DIR} -DBUILD_TESTS=OFF -DBUILD_TRAINING_TOOLS=OFF -DDISABLE_ARCHIVE=ON -DDISABLE_CURL=ON -DFAST_FLOAT=ON -DGRAPHICS_DISABLED=ON -DINSTALL_CONFIGS=OFF -DLeptonica_DIR=${leptonica_BINARY_DIR} -DCMAKE_INSTALL_PREFIX:PATH=${tesseract_BINARY_DIR}/install ${tesseract_SOURCE_DIR} )
        add_custom_target(tess_config ALL DEPENDS tess_config.out)
        add_custom_command(OUTPUT tess_build.out COMMAND cmake --build ${tesseract_BINARY_DIR} --target install )
        add_custom_target(tess_build ALL DEPENDS tess_build.out)
    endif()

    # fetch opencv
    message("fetching https://github.com/opencv/opencv.git")
    FetchContent_Declare(
        opencv
        GIT_REPOSITORY https://github.com/opencv/opencv.git
        GIT_TAG 4.5.4
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    if(WIN64)
        FetchContent_GetProperties(opencv)

        set(BUILD_LIST core,highgui,videoio CACHE STRING "BUILD_LIST")
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
    elseif(LINUX)
        FetchContent_Populate(opencv)

        list(APPEND INCLUDES
            ${opencv_BINARY_DIR}/install/include/opencv4
        )
        list(APPEND LIBS
            ${opencv_BINARY_DIR}/install/lib/libopencv_core.so
            ${opencv_BINARY_DIR}/install/lib/libopencv_highgui.so
            ${opencv_BINARY_DIR}/install/lib/libopencv_imgcodecs.so
            ${opencv_BINARY_DIR}/install/lib/libopencv_imgproc.so
            ${opencv_BINARY_DIR}/install/lib/libopencv_videoio.so
        )

        add_custom_command(OUTPUT opencv_config.out COMMAND cmake -B ${opencv_BINARY_DIR} -DBUILD_LIST=core,highgui,videoio -DBUILD_opencv_python3=OFF -DWITH_IPP=OFF -DBUILD_EXAMPLES=OFF -DOPENCV_GENERATE_PKGCONFIG=ON -DCMAKE_INSTALL_PREFIX:PATH=${opencv_BINARY_DIR}/install ${opencv_SOURCE_DIR} )
        add_custom_target(opencv_config ALL DEPENDS opencv_config.out)
        add_custom_command(OUTPUT opencv_build.out COMMAND cmake --build ${opencv_BINARY_DIR} --target install )
        add_custom_target(opencv_build ALL DEPENDS opencv_build.out)
    endif()

    # fetch rapidjson
    message("fetching  https://github.com/Tencent/rapidjson.git")
    FetchContent_Declare(
        rapidjson
        GIT_REPOSITORY https://github.com/Tencent/rapidjson.git

        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )

    FetchContent_Populate(rapidjson)

    list(APPEND INCLUDES
        ${rapidjson_SOURCE_DIR}/include
    )

    file(GLOB_RECURSE WOLF_ML_OCR_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/ml/referee_ocr/*"
    )

    list(APPEND SRCS
        ${WOLF_ML_OCR_SRC}
    )
endif()