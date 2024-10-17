cmake_minimum_required(VERSION 3.13)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

set(PACKAGE_NAME "react-native-workerbee")


# Debug output
message(STATUS "NODE_MODULES_DIR is set to: ${NODE_MODULES_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR is set to: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR is set to: ${CMAKE_BINARY_DIR}")
message(STATUS "FBJNI_HEADERS_DIR is set to: ${FBJNI_HEADERS_DIR}")
message(STATUS "REACT_NATIVE_DIR is set to: ${REACT_NATIVE_DIR}")
message(STATUS "IS_CLEAN_BUILD is set to: ${IS_CLEAN_BUILD}")

# Ensure FBJNI_HEADERS_DIR is set, otherwise set to a default
if(NOT DEFINED FBJNI_HEADERS_DIR)
    message(WARNING "FBJNI_HEADERS_DIR not set by Gradle. Using a default path.")
    set(FBJNI_HEADERS_DIR "${CMAKE_BINARY_DIR}/generated/source/fbjni-headers")
endif()

# Set the FBJNI include directory based on the header extraction
set(FBJNI_INCLUDE_DIR "${FBJNI_HEADERS_DIR}/prefab/modules/fbjni/include")
message(STATUS "FBJNI_INCLUDE_DIR is set to: ${FBJNI_INCLUDE_DIR}")

# Extracted JNI directory passed by Gradle
set(EXTRACTED_JNI_DIR "${EXTRACTED_JNI_DIR}")
message(STATUS "EXTRACTED_JNI_DIR is set to: ${EXTRACTED_JNI_DIR}")

# Glob the required directories for React Native libraries
file(GLOB LIBRN_DIR "${EXTRACTED_JNI_DIR}/react-native-*/jni/${ANDROID_ABI}")
file(GLOB LIBJSC_DIR "${EXTRACTED_JNI_DIR}/android-jsc*.aar/jni/${ANDROID_ABI}")

# Find package for React Native
find_package(ReactAndroid REQUIRED CONFIG)

set(build_DIR ${CMAKE_SOURCE_DIR}/build)

# Define the library for workerbee JNI
add_library(workerbee-jni SHARED
  ${NODE_MODULES_DIR}/react-native-workerbee/android/src/main/jni/worker-jsi.cpp
  ${NODE_MODULES_DIR}/react-native-workerbee/android/src/main/jni/WorkerBeeModule.cpp
)

# Set include directories for React Native and FBJNI
target_include_directories(workerbee-jni
    PRIVATE
    ${LIBFBJNI_INCLUDE_DIR}
    "${REACT_NATIVE_DIR}/ReactAndroid/src/main/jni/react/turbomodule"
    "${REACT_NATIVE_DIR}/ReactCommon"
    "${REACT_NATIVE_DIR}/ReactCommon/callinvoker"
    "${REACT_NATIVE_DIR}/ReactCommon/jsi/jsi"
    # "${FBJNI_INCLUDE_DIR}"
)

# # Find libraries (React Native JNI, JSC, FBJNI, etc.)
# find_library(
#     FBJNI_LIB
#     fbjni
#     PATHS "${EXTRACTED_JNI_DIR}/jni/${ANDROID_ABI}"
#     NO_CMAKE_FIND_ROOT_PATH
# )

find_package(fbjni REQUIRED CONFIG)

target_link_libraries(workerbee-jni fbjni::fbjni)

# find_library(
#     REACT_NATIVE_JNI_LIB
#     reactnativejni
#     PATHS "${LIBRN_DIR}"
#     NO_CMAKE_FIND_ROOT_PATH
# )

# find_library(
#     REACT_NATIVE_UTILS_LIB
#     reactnativeutilsjni
#     PATHS "${LIBRN_DIR}"
#     NO_CMAKE_FIND_ROOT_PATH
# )

# Handle FBJNI linking based on build type
# if(NOT IS_CLEAN_BUILD)
#     # Regular build: try to find and link the .so file
#     set(FBJNI_LIB_PATH "${EXTRACTED_JNI_DIR}/jni/${ANDROID_ABI}/libfbjni.so")
#     if(EXISTS ${FBJNI_LIB_PATH})
#         message(STATUS "Found FBJNI library at ${FBJNI_LIB_PATH}")
#         target_link_libraries(workerbee-jni ${FBJNI_LIB_PATH})
#     else()
#         message(WARNING "FBJNI library not found at ${FBJNI_LIB_PATH}. Falling back to ReactAndroid::fbjni")
#     endif()
# else()
#     # Clean build: use ReactAndroid::fbjni
#     message(STATUS "Clean build: using ReactAndroid::fbjni")
# endif()

# Link libraries
target_link_libraries(workerbee-jni
    ${LOG_LIB}
    android
    ReactAndroid::jsi
    ReactAndroid::reactnativejni
    ReactAndroid::react_nativemodule_core
    # ${FBJNI_LIB}       # Link extracted FBJNI .so
    ${REACT_NATIVE_JNI_LIB}
    ${REACT_NATIVE_UTILS_LIB}
)

# Additional debug output
message(STATUS "JSI include path: ${REACT_NATIVE_DIR}/ReactCommon/jsi/jsi")
message(STATUS "FBJNI include path: ${FBJNI_INCLUDE_DIR}")

# Check if jsi.h exists in the specified directory
if(EXISTS "${REACT_NATIVE_DIR}/ReactCommon/jsi/jsi/jsi.h")
    message(STATUS "jsi.h found in ${REACT_NATIVE_DIR}/ReactCommon/jsi/jsi/jsi.h")
else()
    message(WARNING "jsi.h not found in ${REACT_NATIVE_DIR}/ReactCommon/jsi/jsi/jsi.h")
    file(GLOB_RECURSE JSI_FILES "${REACT_NATIVE_DIR}/ReactCommon/jsi/jsi/*")
    foreach(FILE ${JSI_FILES})
        message(STATUS "Found file: ${FILE}")
    endforeach()
endif()

# Check if fbjni.h exists in the specified directory
if(EXISTS "${FBJNI_INCLUDE_DIR}/fbjni/fbjni.h")
    message(STATUS "fbjni.h found in ${FBJNI_INCLUDE_DIR}/fbjni/fbjni.h")
else()
    message(WARNING "fbjni.h not found in ${FBJNI_INCLUDE_DIR}/fbjni/fbjni.h")
    file(GLOB_RECURSE FBJNI_FILES "${FBJNI_INCLUDE_DIR}/*")
    foreach(FILE ${FBJNI_FILES})
        message(STATUS "Found file: ${FILE}")
    endforeach()
endif()

# Add fbjni to CMAKE_PREFIX_PATH (just in case it's needed elsewhere)
list(APPEND CMAKE_PREFIX_PATH ${FBJNI_HEADERS_DIR})
message(STATUS "CMAKE_PREFIX_PATH is now: ${CMAKE_PREFIX_PATH}")
