cmake_minimum_required(VERSION 3.13)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

# Debug output
message(STATUS "NODE_MODULES_DIR is set to: ${NODE_MODULES_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR is set to: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR is set to: ${CMAKE_BINARY_DIR}")
message(STATUS "FBJNI_HEADERS_DIR is set to: ${FBJNI_HEADERS_DIR}")

if(NOT DEFINED FBJNI_HEADERS_DIR)
    message(WARNING "FBJNI_HEADERS_DIR not set by Gradle. Using a default path.")
    set(FBJNI_HEADERS_DIR "${CMAKE_BINARY_DIR}/generated/source/fbjni-headers")
endif()

# Set the correct path for fbjni include directory
set(FBJNI_INCLUDE_DIR "${FBJNI_HEADERS_DIR}/prefab/modules/fbjni/include")
message(STATUS "FBJNI_INCLUDE_DIR is set to: ${FBJNI_INCLUDE_DIR}")

# Set fbjni_DIR
set(fbjni_DIR ${FBJNI_INCLUDE_DIR})
message(STATUS "fbjni_DIR is set to: ${fbjni_DIR}")

# Define the library
add_library(workerbee-jni SHARED
  ${NODE_MODULES_DIR}/react-native-workerbee/android/src/main/jni/worker-jsi.cpp
  ${NODE_MODULES_DIR}/react-native-workerbee/android/src/main/jni/WorkerBeeModule.cpp
)

# Set include directories for React Native, Hermes, and fbjni
target_include_directories(workerbee-jni
    PRIVATE
    "${NODE_MODULES_DIR}/react-native/ReactCommon"
    "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni/react/turbomodule"
    "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
    "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
    "${NODE_MODULES_DIR}/react-native/ReactCommon/hermes/executor"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${FBJNI_INCLUDE_DIR}"
)

# Find required libraries
find_library(LOG_LIB log)

# Link libraries
target_link_libraries(workerbee-jni
    android
    ${LOG_LIB}
    jsi
    react_nativemodule_core
)

# Additional debug output
message(STATUS "JSI include path: ${NODE_MODULES_DIR}/react-native/ReactCommon/jsi")
message(STATUS "FBJNI include path: ${FBJNI_INCLUDE_DIR}")

# Check if fbjni.h exists in the specified directory
if(EXISTS "${FBJNI_INCLUDE_DIR}/fbjni/fbjni.h")
    message(STATUS "fbjni.h found in ${FBJNI_INCLUDE_DIR}/fbjni/fbjni.h")
else()
    message(WARNING "fbjni.h not found in ${FBJNI_INCLUDE_DIR}/fbjni/fbjni.h")
    # List contents of FBJNI_INCLUDE_DIR for debugging
    file(GLOB_RECURSE FBJNI_FILES "${FBJNI_INCLUDE_DIR}/*")
    foreach(FILE ${FBJNI_FILES})
        message(STATUS "Found file: ${FILE}")
    endforeach()
endif()

# Add fbjni to CMAKE_PREFIX_PATH (just in case it's needed elsewhere)
list(APPEND CMAKE_PREFIX_PATH ${FBJNI_HEADERS_DIR})
message(STATUS "CMAKE_PREFIX_PATH is now: ${CMAKE_PREFIX_PATH}")