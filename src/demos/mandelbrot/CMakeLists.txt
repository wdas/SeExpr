if(NOT APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")

add_executable(mandelbrot-control mandelbrot-control.cpp)
install(TARGETS mandelbrot-control DESTINATION share/test/SeExpr2)

add_executable(mandelbrot-control2 mandelbrot-control2.cpp)
install(TARGETS mandelbrot-control2 DESTINATION share/test/SeExpr2)

add_executable(mandelbrot-seexpr2 mandelbrot-seexpr2.cpp)
install(TARGETS mandelbrot-seexpr2 DESTINATION share/test/SeExpr2)
target_link_libraries(mandelbrot-seexpr2 SeExpr2)

add_executable(mandelbrot-seexpr2-varblock mandelbrot-seexpr2-varblock.cpp)
install(TARGETS mandelbrot-seexpr2-varblock DESTINATION share/test/SeExpr2)
target_link_libraries(mandelbrot-seexpr2-varblock SeExpr2)

add_executable(mandelbrot-seexpr2-varblock2 mandelbrot-seexpr2-varblock2.cpp)
install(TARGETS mandelbrot-seexpr2-varblock2 DESTINATION share/test/SeExpr2)
target_link_libraries(mandelbrot-seexpr2-varblock2 SeExpr2)

add_executable(mandelbrot-seexpr2-varblock3 mandelbrot-seexpr2-varblock3.cpp)
install(TARGETS mandelbrot-seexpr2-varblock3 DESTINATION share/test/SeExpr2)
target_link_libraries(mandelbrot-seexpr2-varblock3 SeExpr2)
