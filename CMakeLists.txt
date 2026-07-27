cmake_minimum_required( VERSION 3.28 FATAL_ERROR )

project( pimpl
    VERSION 0.1.0
    DESCRIPTION ""
    LANGUAGES CXX
)

set( HEADERS
    include/pimpl/dynamic_provider.h
    include/pimpl/fast_pimpl.h
    include/pimpl/fixed_provider.h
    include/pimpl/pimpl.h
    include/pimpl/pimpl_holder.h
    include/pimpl/provider.h
    include/pimpl/type_erasure.h
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

target_sources( ${PROJECT_NAME} 
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/pimpl/pimpl.natvis>
)

add_subdirectory( tests )