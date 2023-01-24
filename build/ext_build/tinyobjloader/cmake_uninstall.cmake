if(NOT EXISTS "/Users/benoitboidin/Desktop/s8_info/mondes_3d/tp3/build/ext_build/tinyobjloader/install_manifest.txt")
  message(FATAL_ERROR "Cannot find install manifest: /Users/benoitboidin/Desktop/s8_info/mondes_3d/tp3/build/ext_build/tinyobjloader/install_manifest.txt")
endif(NOT EXISTS "/Users/benoitboidin/Desktop/s8_info/mondes_3d/tp3/build/ext_build/tinyobjloader/install_manifest.txt")

file(READ "/Users/benoitboidin/Desktop/s8_info/mondes_3d/tp3/build/ext_build/tinyobjloader/install_manifest.txt" files)
string(REGEX REPLACE "\n" ";" files "${files}")
foreach(file ${files})
  message(STATUS "Uninstalling $ENV{DESTDIR}${file}")
  if(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    exec_program(
      "/usr/local/Cellar/cmake/3.25.1/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
      OUTPUT_VARIABLE rm_out
      RETURN_VALUE rm_retval
      )
    if(NOT "${rm_retval}" STREQUAL 0)
      message(FATAL_ERROR "Problem when removing $ENV{DESTDIR}${file}")
    endif(NOT "${rm_retval}" STREQUAL 0)
  else(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
    message(STATUS "File $ENV{DESTDIR}${file} does not exist.")
  endif(IS_SYMLINK "$ENV{DESTDIR}${file}" OR EXISTS "$ENV{DESTDIR}${file}")
endforeach(file)