add_library(MathFunctions STATIC mysqrt.cxx)
add_library(virtual_library INTERFACE)

target_compile_features(virtual_library INTERFACE cxx_std_20)
target_include_directories(MathFunctions PUBLIC "${CMAKE_CURRENT_LIST_DIR}")
target_link_libraries(MathFunctions PUBLIC virtual_library)


set(installable_libs MathFunctions virtual_library)
install(TARGETS ${installable_libs} DESTINATION lib)
install(FILES mysqrt.h DESTINATION include)