# CMakeList.txt : CMake project for DearGlfw, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(DearGlfw "DearGlfw.cpp" "DearGlfw.h")
find_package(SDL2 CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(sdl2-gfx CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(Tweeny CONFIG REQUIRED)

target_link_libraries(DearGlfw PRIVATE SDL2::SDL2_ttf)
target_link_libraries(DearGlfw PRIVATE glfw)
target_link_libraries(DearGlfw PRIVATE imgui::imgui)
target_link_libraries(DearGlfw PRIVATE SDL2::SDL2 SDL2::SDL2main)
target_link_libraries(DearGlfw PRIVATE SDL2::SDL2_gfx)
target_link_libraries(DearGlfw PRIVATE fmt::fmt)
target_link_libraries(DearGlfw PRIVATE SDL2::SDL2_image)
target_link_libraries(DearGlfw PRIVATE tweeny)
# TODO: Add tests and install targets if needed.
