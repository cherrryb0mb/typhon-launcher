#
#
# Try to find the P3T libraries
# Once done this will define
#
# P3T_FOUND          - system has p3t
# P3T_INCLUDE_DIR    - path to p3t/p3t.h
# P3T_LIBRARY      - the library that must be included
#
#

IF (P3T_LIBRARY AND P3T_INCLUDE_DIR)
  SET(P3T_FOUND "YES")
ELSE (P3T_LIBRARY AND P3T_INCLUDE_DIR)
  FIND_PATH(P3T_INCLUDE_DIR p3t.h PATHS /usr/local/include/p3t /usr/include/p3t)
  FIND_LIBRARY(P3T_LIBRARY p3t PATHS /usr/local/lib /usr/lib)
  
  IF (P3T_INCLUDE_DIR AND P3T_LIBRARY)
    SET(P3T_FOUND "YES")
  ELSE (P3T_INCLUDE_DIR AND P3T_LIBRARY)
    SET(P3T_FOUND "NO")
  ENDIF (P3T_INCLUDE_DIR AND P3T_LIBRARY)
ENDIF (P3T_LIBRARY AND P3T_INCLUDE_DIR)

IF (P3T_FOUND)
  MESSAGE(STATUS "Found P3T libraries at ${P3T_LIBRARY} and includes at ${P3T_INCLUDE_DIR}")
ELSE (P3T_FOUND)
  IF (P3T_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find P3T libraries")
  ENDIF (P3T_FIND_REQUIRED)
ENDIF (P3T_FOUND)
