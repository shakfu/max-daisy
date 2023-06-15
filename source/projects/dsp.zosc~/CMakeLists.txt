include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-pretarget.cmake)
set(CMAKE_VERBOSE_MAKEFILE ON)

#############################################################
# MAX EXTERNAL
#############################################################

set(DAISY ${CMAKE_SOURCE_DIR}/source/projects/daisy_sp)
set(DAISY_INCLUDE
    ${DAISY}
    ${DAISY}/Control
    ${DAISY}/Drums
    ${DAISY}/Dynamics
    ${DAISY}/Effects
    ${DAISY}/Filters
    ${DAISY}/Noise
    ${DAISY}/PhysicalModeling
    ${DAISY}/Synthesis
    ${DAISY}/Utility
)

include_directories( 
  "${MAX_SDK_INCLUDES}"
  "${MAX_SDK_MSP_INCLUDES}"
  "${MAX_SDK_JIT_INCLUDES}"
)

file(GLOB PROJECT_SRC
   "*.h"
   "*.c"
   "*.cpp"
)

add_library( 
    ${PROJECT_NAME} 
    MODULE
    ${PROJECT_SRC}
)


target_include_directories(${PROJECT_NAME}
    PUBLIC
    ${DAISY_INCLUDE}
)


target_link_libraries(${PROJECT_NAME}
    PUBLIC
    DaisySP
)



include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-base/script/max-posttarget.cmake)
