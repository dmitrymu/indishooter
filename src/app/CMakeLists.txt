find_package(Boost REQUIRED COMPONENTS program_options)

add_compile_definitions(PROJECT_VERISON="${PROJECT_VERSION}")

add_executable(indishooter
               main.cpp
               options.cpp)

target_link_libraries(indishooter 
                      shooter
                      Boost::program_options)
