message(STATUS "Fetching fftw3...")

FetchContent_MakeAvailable(fftw3)

target_include_directories(${PROJECT_NAME} PRIVATE ${fftw3_SOURCE_DIR}/api)
