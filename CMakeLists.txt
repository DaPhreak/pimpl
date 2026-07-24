cmake_minimum_required( VERSION 3.28 FATAL_ERROR )

project( fast_pimpl
    VERSION 0.1.0
    DESCRIPTION ""
    LANGUAGES CXX
)

set( HEADERS
    include/fast_pimpl/fast_pimpl.h
    include/fast_pimpl/fixed_allocator.h
    include/fast_pimpl/pimpl.h
)

add_library( ${PROJECT_NAME} INTERFACE
    ${HEADERS}
)

target_compile_features( ${PROJECT_NAME} 
    INTERFACE 
        cxx_std_17
)

target_include_directories( ${PROJECT_NAME} 
    INTERFACE 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_subdirectory( tests )