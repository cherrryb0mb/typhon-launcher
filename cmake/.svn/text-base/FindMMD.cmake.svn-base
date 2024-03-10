#
#
# Try to find the MMD libraries
# Once done this will define
#
# MMD_FOUND          - system has mmd
# MMD_INCLUDE_DIR    - path to mmd/PMDModel.h
# MMD_LIBRARY      - the library that must be included
#
#

IF (MMD_LIBRARY AND MMD_INCLUDE_DIR)
  SET(MMD_FOUND "YES")
ELSE (MMD_LIBRARY AND MMD_INCLUDE_DIR)
  FIND_PATH(MMD_INCLUDE_DIR PMDModel.h PATHS /usr/local/include/mmd /usr/include/mmd)
  FIND_LIBRARY(MMD_LIBRARY mmd PATHS /usr/local/lib /usr/lib)
  
  IF (MMD_INCLUDE_DIR AND MMD_LIBRARY)
    SET(MMD_FOUND "YES")
  ELSE (MMD_INCLUDE_DIR AND MMD_LIBRARY)
    SET(MMD_FOUND "NO")
  ENDIF (MMD_INCLUDE_DIR AND MMD_LIBRARY)
ENDIF (MMD_LIBRARY AND MMD_INCLUDE_DIR)

IF (MMD_FOUND)
  MESSAGE(STATUS "Found MMD libraries at ${MMD_LIBRARY} and includes at ${MMD_INCLUDE_DIR}")
ELSE (MMD_FOUND)
  IF (MMD_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find MMD libraries")
  ENDIF (MMD_FIND_REQUIRED)
ENDIF (MMD_FOUND)
