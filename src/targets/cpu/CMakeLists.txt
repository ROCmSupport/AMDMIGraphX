
add_library(migraphx_cpu
    allocation_model.cpp
    allocate.cpp
    add.cpp
    contiguous.cpp
    concat.cpp
    convolution.cpp
    copy.cpp
    mul.cpp
    pooling.cpp
    relu.cpp
    gemm.cpp
    target.cpp
    lowering.cpp
    migemm.cpp
)
set_target_properties(migraphx_cpu PROPERTIES EXPORT_NAME cpu)
rocm_set_soversion(migraphx_cpu ${MIGRAPHX_SO_VERSION})

find_path(BLAZE_INCLUDE blaze/Blaze.h)
find_package(Threads)
find_package(dnnl REQUIRED)

rocm_clang_tidy_check(migraphx_cpu)
target_link_libraries(migraphx_cpu PRIVATE migraphx Threads::Threads)
target_include_directories(migraphx_cpu PRIVATE ${BLAZE_INCLUDE})
target_compile_definitions(migraphx_cpu PRIVATE -DBLAZE_USE_CPP_THREADS)
target_link_libraries(migraphx_cpu PRIVATE DNNL::dnnl)
target_compile_definitions(migraphx_cpu PUBLIC -DUSE_DNNL)
find_package(OpenMP)
target_link_libraries(migraphx_cpu PUBLIC OpenMP::OpenMP_CXX)
# Add library path to rpath to workaround issues with our broken packages
foreach(LIBRARY ${OpenMP_CXX_LIBRARIES})
    if(LIBRARY MATCHES "libomp")
        get_filename_component(LIBRARY_PATH "${LIBRARY}" PATH)
        target_link_libraries(migraphx_cpu PUBLIC -Wl,-rpath=${LIBRARY_PATH} -Wl,-rpath-link=${LIBRARY_PATH})
    endif()
endforeach()

target_link_libraries(migraphx_all_targets INTERFACE migraphx_cpu)

rocm_install_targets(
  TARGETS migraphx_cpu
  INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

