get_filename_component(Purify_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
message(STATUS "Linking to purify package in ${Purify_CMAKE_DIR}")
if(NOT TARGET libpurify AND EXISTS "${Purify_CMAKE_DIR}/PurifyTargets.cmake")
  include("${Purify_CMAKE_DIR}/PurifyTargets.cmake")
endif()

set(Purify_INCLUDE_DIRS "@ALL_INCLUDE_DIRS@")
set(Purify_LIBRARIES libpurify)
set(Purify_EXECUTABLE purify)
