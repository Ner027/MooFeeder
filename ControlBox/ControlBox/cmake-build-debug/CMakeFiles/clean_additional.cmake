# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/ControlBox_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ControlBox_autogen.dir/ParseCache.txt"
  "ControlBox_autogen"
  )
endif()
