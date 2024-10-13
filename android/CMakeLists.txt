cmake_minimum_required(VERSION 3.13)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

# Debug output
message(STATUS "NODE_MODULES_DIR is set to: ${NODE_MODULES_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR is set to: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "BUILD_DIR is set to: ${CMAKE_BINARY_DIR}")

# Define the library
add_library(workerbee-jni SHARED
  ${NODE_MODULES_DIR}/react-native-workerbee/android/src/main/jni/worker-jsi.cpp
  ${NODE_MODULES_DIR}/react-native-workerbee/android/src/main/jni/WorkerBeeModule.cpp # Include any other C++ source files here
)

find_package(fbjni REQUIRED CONFIG)

# Set include directories for React Native and Hermes
target_include_directories(workerbee-jni
    PRIVATE
    "${NODE_MODULES_DIR}/react-native/ReactCommon"
    "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni/react/turbomodule"
    "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
    "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
    "${NODE_MODULES_DIR}/react-native/ReactCommon/hermes/executor"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_BINARY_DIR}/workerbee/headers"   # Include the extracted fbjni headers from the Gradle task
)

# Find required libraries
find_library(LOG_LIB log)

# Link libraries
target_link_libraries(workerbee-jni
    android
    ${LOG_LIB}
    jsi
    fbjni::fbjni
    react_nativemodule_core
)

# Additional debug output
message(STATUS "JSI include path: ${NODE_MODULES_DIR}/react-native/ReactCommon/jsi")
message(STATUS "FBJNI include path: ${CMAKE_BINARY_DIR}/workerbee/headers")
