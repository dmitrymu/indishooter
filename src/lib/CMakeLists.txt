add_library(shooter
    ShooterClient.cpp
    FilenameGenerator.cpp
    )

message(INDI_INCLUDE_DIR="${INDI_INCLUDE_DIR}")    
message(INDI_LIBRARIES="${INDI_LIBRARIES}")    

target_include_directories(shooter
  PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/../include
          ${INDI_INCLUDE_DIR}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    
target_link_libraries(shooter
                      ${INDI_LIBRARIES})