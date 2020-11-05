cmake_minimum_required (VERSION 3.5)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)

if (NOT DEFINED COMPONENT_DIR)

    project (lcdgfx)

    include_directories(src)

    add_library(ssd1306 STATIC ${HEADER_FILES} ${SOURCE_FILES})

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS "src")

endif()
