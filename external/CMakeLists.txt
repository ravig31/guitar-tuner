include(FetchContent)

FetchContent_Declare(
  portaudio
  GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
  GIT_TAG v19.7.0
)

add_subdirectory(portaudio)


FetchContent_Declare(
  fftw3
  URL https://fftw.org/fftw-3.3.10.tar.gz
  URL_HASH MD5=8ccbf6a5ea78a16dbc3e1306e234cc5c
)

add_subdirectory(fftw3)