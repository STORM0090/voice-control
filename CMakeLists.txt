cmake_minimum_required(VERSION 3.15)
project(VoiceControl LANGUAGES C CXX)

# Use C++17.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# whisper.cpp: link it statically and skip its examples and tests.
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(WHISPER_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(WHISPER_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/whisper.cpp)

add_executable(VoiceControl
    src/main.cpp
    src/audio/AudioCapture.cpp
    src/audio/Resampler.cpp
    src/vad/Vad.cpp
    src/stt/Transcriber.cpp
    src/command/CommandParser.cpp
    src/action/ActionExecutor.cpp
    src/tts/Speaker.cpp
)

# Let the code write includes like "audio/AudioCapture.h" and "common/AudioTypes.h".
target_include_directories(VoiceControl PRIVATE src)

# COM (CoInitializeEx, CoCreateInstance) lives in ole32.
target_link_libraries(VoiceControl PRIVATE ole32 whisper)

# With CUDA the exe needs cuBLAS/cudart DLLs at runtime, so copy them
# next to it. Configure with -DGGML_CUDA=ON to enable GPU support.
if (GGML_CUDA)
    file(GLOB CUDA_DLLS
        "$ENV{CUDA_PATH}/bin/x64/cudart64_*.dll"
        "$ENV{CUDA_PATH}/bin/x64/cublas64_*.dll"
        "$ENV{CUDA_PATH}/bin/x64/cublasLt64_*.dll"
    )
    foreach(dll ${CUDA_DLLS})
        add_custom_command(TARGET VoiceControl POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${dll}" "$<TARGET_FILE_DIR:VoiceControl>")
    endforeach()
endif()
