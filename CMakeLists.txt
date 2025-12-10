cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(menu)

set(VERSIONS 1 2 3 4 5 6 7)

add_library(menu SHARED src/main.cc src/layers/HaxMenu.cpp src/version/VersionUtils.cpp src/Utils.cpp)

set(DOBBY_DEBUG OFF CACHE INTERNAL "" FORCE)
set(DOBBY_GENERATE_SHARED OFF CACHE INTERNAL "" FORCE)
add_definitions(-DLOGGING_DISABLE -DNDEBUG)
add_subdirectory(vendor/Dobby)

target_compile_options(menu PUBLIC -fdebug-prefix-map=${CMAKE_SOURCE_DIR}=.)

target_link_libraries(menu dobby)

add_definitions(-DGAME_VERSION=${GAME_VERSION})
if (NOT GAME_VERSION IN_LIST VERSIONS)
    message(FATAL_ERROR "GAME_VERSION needs to be defined. The possible values are: 1 (1.02), 2 (1.11), 3 (1.22), 4 (1.3), 5 (1.41), 6 (1.51), 7 (1.6)")
endif()

if(GAME_VERSION LESS 6)
    set(ANDROID_ABI armeabi)
else()
    set(ANDROID_ABI armeabi-v7a)
endif()

if(GAME_VERSION EQUAL 1)
    message("Linking with 1.0") 
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.0/libgame.so log android)
elseif(GAME_VERSION EQUAL 2)
    message("Linking with 1.1") 
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.1/libgame.so log android)
elseif(GAME_VERSION EQUAL 3)
    message("Linking with 1.2") 
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.2/libgame.so log android)
elseif(GAME_VERSION EQUAL 4)
    message("Linking with 1.3")
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.3/libgame.so log android)
elseif(GAME_VERSION EQUAL 5)
    message("Linking with 1.4")
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.4/libgame.so log android)
elseif(GAME_VERSION EQUAL 6)
    message("Linking with 1.5")
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.5/libgame.so log android)
elseif(GAME_VERSION EQUAL 7)
    message("Linking with 1.6")
    target_link_libraries(menu ${CMAKE_CURRENT_SOURCE_DIR}/src/version/1.6/libgame.so log android)
endif()

target_include_directories(menu PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cocos2d/cocos2dx/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cocos2d/cocos2dx
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cocos2d/cocos2dx/platform/android
    ${CMAKE_CURRENT_SOURCE_DIR}/include/cocos2d/cocos2dx/kazmath/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/layers
    ${CMAKE_CURRENT_SOURCE_DIR}/include/robtop
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/layers
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hooks
    ${CMAKE_CURRENT_SOURCE_DIR}/src/version)