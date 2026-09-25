cmake_minimum_required(VERSION 3.10)

project(TinyCThread
        VERSION 1.0.0
        DESCRIPTION "libGimbal fork of TinyCThread"
        LANGUAGES C)
enable_testing()

option(TINYCTHREAD_ENABLE_THREADS "Build with threading support" ON)
option(TINYCTHREAD_ENABLE_TESTS "Enable TinyCThread unit tests" OFF)
option(TINYCTHREAD_INSTALL "Install a static library for TinyCThread")
option(TINYCTHREAD_USE_C11_THREADS "Force use of C11 <threads.h> backend" OFF)
option(TINYCTHREAD_TLS_EMULATED "Emulate compiler TLS with TSS" OFF)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(tinycthread STATIC
    source/tinycthread.c
    source/tinycthread.h
    source/tinycthread_tls.h)

if(TINYCTHREAD_ENABLE_THREADS AND NOT DEFINED PLATFORM_DREAMCAST)
    find_package(Threads REQUIRED)
    target_link_libraries(tinycthread ${CMAKE_THREAD_LIBS_INIT})
endif()

set_property(TARGET tinycthread APPEND PROPERTY INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/source")
set_property(TARGET tinycthread APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/source")
set_property(TARGET tinycthread PROPERTY POSITION_INDEPENDENT_CODE True)

target_compile_definitions(tinycthread
    PUBLIC
      $<$<BOOL:${TINYCTHREAD_USE_C11_THREADS}>:TINYCTHREAD_FORCE_C11>
      TINYCTHREAD_ENABLE_THREADS=$<BOOL:${TINYCTHREAD_ENABLE_THREADS}>
      TTHREAD_TLS_EMULATED=$<BOOL:${TINYCTHREAD_TLS_EMULATED}>
)

if(TINYCTHREAD_ENABLE_TESTS AND TINYCTHREAD_ENABLE_THREADS)
  add_executable(test-tinycthread "${CMAKE_CURRENT_SOURCE_DIR}/test/test.c")
  target_link_libraries(test-tinycthread tinycthread)

  add_test(NAME tinycthread
    COMMAND $<TARGET_FILE:test-tinycthread>)
endif()

if(TINYCTHREAD_ENABLE_TESTS AND NOT TINYCTHREAD_ENABLE_THREADS)
  message(STATUS "TinyCThread tests require threading and will not be built")
endif()


if(TINYCTHREAD_INSTALL)
  if(CMAKE_INSTALL_LIBDIR)
    install(TARGETS tinycthread ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  else()
    install(TARGETS tinycthread ARCHIVE DESTINATION lib)
  endif()

  if(CMAKE_INSTALL_INCLUDEDIR)
    install(FILES source/tinycthread.h source/tinycthread_tls.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
  else()
    install(FILES source/tinycthread.h source/tinycthread_tls.h DESTINATION include)
  endif()
endif(TINYCTHREAD_INSTALL)
