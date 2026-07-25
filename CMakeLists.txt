cmake_minimum_required(VERSION 3.15)
project(VoiceControl LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# link whisper.cpp statically, skip its examples and tests
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
    src/action/VolumeControl.cpp
    src/tts/Speaker.cpp
)

target_include_directories(VoiceControl PRIVATE src)

target_link_libraries(VoiceControl PRIVATE ole32 whisper) # ole32 = COM

# the CUDA build needs these DLLs next to the exe at runtime
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
