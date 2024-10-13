cmake_minimum_required(VERSION 3.4.1)

# Set your library name
add_library(workerbee-jni SHARED
    jni/worker-jsi.cpp
    jni/WorkerBeeModule.cpp # Include any other C++ source files here
)

# Find the required libraries
find_library(log-lib log)

# Link libraries
target_link_libraries(workerbee-jni
    ${log-lib}
    jsi # Link to JSI if you are using it
    # You can add other dependencies here if needed
)
