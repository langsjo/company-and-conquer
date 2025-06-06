project(main VERSION 0.1.0)
enable_language(CXX)

add_subdirectory(backend)
add_subdirectory(frontend)

# Add all .c, - files under src/ directory
file(GLOB SOURCES_C "*.c")
# Add all .cpp, - files under src/ directory
file(GLOB SOURCES_CPP "*.cpp")
# Add all .hpp, - files under src/ directory
file(GLOB SOURCES_HPP "*.hpp")
# Add all .h, - files under src/ directory
file(GLOB SOURCES_H "*.h")

set(SOURCES ${SOURCES_C} ${SOURCES_CPP} ${SOURCES_H} ${SOURCES_HPP})


# add the executable target
add_executable(main)

target_sources(main PUBLIC ${SOURCES})
target_link_libraries(main PUBLIC sfml-system sfml-window sfml-graphics sfml-network sfml-audio)
target_compile_features(main PRIVATE cxx_std_20)
target_link_libraries(main PRIVATE backend)
target_include_directories(main PRIVATE backend)
set_target_properties(backend PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(main PRIVATE frontend)
target_include_directories(main PRIVATE frontend)
set_target_properties(frontend PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(main PRIVATE ../libs/tinyfiledialogs)
target_link_libraries(main PRIVATE yaml-cpp::yaml-cpp)


# For relative paths.
    # Maps
set(TESTMAP_PATH ${CMAKE_SOURCE_DIR}/tests/test_map.txt)
add_compile_definitions(TESTMAP_PATH="${TESTMAP_PATH}")
set(TESTMAP_PATH1 ${CMAKE_SOURCE_DIR}/tests/test_map1.txt)
add_compile_definitions(TESTMAP_PATH1="${TESTMAP_PATH1}")
set(TEXTURE_PATH ${CMAKE_SOURCE_DIR}/textures/tiles.png)
add_compile_definitions(TEXTURE_PATH="${TEXTURE_PATH}")
    # Units
set(UNITS_TEXTURE_PATH ${CMAKE_SOURCE_DIR}/textures/units.png)
add_compile_definitions(UNITS_TEXTURE_PATH="${UNITS_TEXTURE_PATH}")
    # Buildings
set(BUILDINGS_TEXTURE_PATH ${CMAKE_SOURCE_DIR}/textures/buildings.png)
add_compile_definitions(BUILDINGS_TEXTURE_PATH="${BUILDINGS_TEXTURE_PATH}")
    # Aux
set(AUX_TEXTURE_PATH ${CMAKE_SOURCE_DIR}/textures/_aux.png)
add_compile_definitions(AUX_TEXTURE_PATH="${AUX_TEXTURE_PATH}")


# set(INVENTORY_TEXTURE_PATH ${CMAKE_SOURCE_DIR}/textures/inventory.png)
# add_compile_definitions(INVENTORY_TEXTURE_PATH="${INVENTORY_TEXTURE_PATH}")

    # Font
set(FONT_PATH ${CMAKE_SOURCE_DIR}/textures/font.TTF)
add_compile_definitions(FONT_PATH="${FONT_PATH}")

# get a lot of warnings
if(MSVC)
    target_compile_options(main PRIVATE -std=c++20 /Wall)
else()
    # gcc and clang
    target_compile_options(main PRIVATE -std=c++20 -Wall -Wextra -pedantic -Wno-missing-field-initializers)
endif()

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()
