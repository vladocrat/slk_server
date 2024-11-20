include(FetchContent)

FetchContent_Declare(
  libpqxx
  GIT_REPOSITORY https://github.com/jtv/libpqxx.git
  GIT_TAG 1ca80b0e638f6182426c5b11255069cae4fbd542
)
set(PQXX_LIBRARIES pqxx_static)

FetchContent_MakeAvailable(libpqxx)
