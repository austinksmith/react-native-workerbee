cmake_minimum_required(VERSION 3.4.1)

# Define your project name and version
project(reactnativeworkerbee)

# Set your library name and specify source files
add_library(workerbee-jni SHARED
    src/main/jni/worker-jsi.cpp
    src/main/jni/WorkerBeeModule.cpp # Include any other C++ source files here
)

# Include directories for React Native and your JNI folder
include_directories(${CMAKE_SOURCE_DIR}/../..)
include_directories(${CMAKE_SOURCE_DIR}/src/main/jni)  # Ensure this points to your JNI folder

# Find the required Android libraries
find_library(log-lib log)

# Link against the required libraries
target_link_libraries(workerbee-jni
    ${log-lib}  # Link the Android log library
    jsi         # Link to JSI if you are using it
    fbjni       # Link to FBJNI, needed for JNI binding
    # Add other dependencies here if needed
)
