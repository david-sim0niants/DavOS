include(FetchContent)

set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY 	https://github.com/catchorg/Catch2.git
  GIT_TAG 		v3.4.0
  GIT_PROGRESS 		TRUE
)

FetchContent_MakeAvailable(Catch2)

add_subdirectory(unit)
