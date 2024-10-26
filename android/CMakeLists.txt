cmake_minimum_required(VERSION 3.13)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

set(PACKAGE_NAME "react-native-workerbee")

# Debug output
message(STATUS "CMAKE_CURRENT_SOURCE_DIR is set to: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR is set to: ${CMAKE_BINARY_DIR}")
message(STATUS "REACT_NATIVE_DIR is set to: ${REACT_NATIVE_DIR}")

# Find package for React Native
find_package(ReactAndroid REQUIRED CONFIG)
find_package(fbjni REQUIRED CONFIG)

# Define the library for workerbee JNI
add_library(workerbee-jni SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main/jni/WorkerBeeModule.cpp
)

# Set include directories for React Native and FBJNI
target_include_directories(workerbee-jni
    PRIVATE
    "${REACT_NATIVE_DIR}/ReactAndroid/src/main/jni/react/turbomodule"
    "${REACT_NATIVE_DIR}/ReactCommon"
    "${REACT_NATIVE_DIR}/ReactCommon/callinvoker"
    "${REACT_NATIVE_DIR}/ReactCommon/jsi"
)

# Link libraries
target_link_libraries(workerbee-jni
    android
    ReactAndroid::jsi
    ReactAndroid::fabricjni
    ReactAndroid::reactnativejni
    ReactAndroid::react_nativemodule_core
    fbjni::fbjni
)

message(STATUS "CMAKE_PREFIX_PATH is now: ${CMAKE_PREFIX_PATH}")