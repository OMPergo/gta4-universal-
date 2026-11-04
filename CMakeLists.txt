cmake_minimum_required(VERSION 3.20)
project(LibertyCityUniversal LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/LibertyRecomp
    ${CMAKE_CURRENT_SOURCE_DIR}/LibertyRecomp/diagnostics
    ${CMAKE_CURRENT_SOURCE_DIR}/LibertyRecomp/gpu
    ${CMAKE_CURRENT_SOURCE_DIR}/LibertyRecomp/network
    ${CMAKE_CURRENT_SOURCE_DIR}/installer
)

# Core library
set(LIBERTY_CORE_SOURCES
    LibertyRecomp/universal_runtime.cpp
    LibertyRecomp/diagnostics/hardware_capability.cpp
    LibertyRecomp/diagnostics/performance_tiers.cpp
    LibertyRecomp/gpu/gpu_compatibility.cpp
    LibertyRecomp/network/authoritative_session.cpp
    installer/media_id_validator.cpp
)

if(ANDROID)
    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})
    list(APPEND LIBERTY_CORE_SOURCES LibertyRecomp/jni/universal_jni_bridge.cpp)

    # Build shared library for Android packaging
    add_library(LibertyUniversalCore SHARED ${LIBERTY_CORE_SOURCES})
    target_link_libraries(LibertyUniversalCore PRIVATE log android)
else()
    # Build static library for host unit tests
    add_library(LibertyUniversalCore STATIC ${LIBERTY_CORE_SOURCES})
endif()

# Find Vulkan if available
find_package(Vulkan QUIET)
if(Vulkan_FOUND)
    target_include_directories(LibertyUniversalCore PUBLIC ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(LibertyUniversalCore PUBLIC ${Vulkan_LIBRARIES})
    target_compile_definitions(LibertyUniversalCore PUBLIC HAVE_VULKAN=1)
endif()

# Unit tests
enable_testing()

add_executable(test_hardware_capability tests/unit/test_hardware_capability.cpp)
target_link_libraries(test_hardware_capability PRIVATE LibertyUniversalCore)
add_test(NAME HardwareCapabilityTest COMMAND test_hardware_capability)

add_executable(test_performance_tiers tests/unit/test_performance_tiers.cpp)
target_link_libraries(test_performance_tiers PRIVATE LibertyUniversalCore)
add_test(NAME PerformanceTiersTest COMMAND test_performance_tiers)

add_executable(test_gpu_compatibility tests/unit/test_gpu_compatibility.cpp)
target_link_libraries(test_gpu_compatibility PRIVATE LibertyUniversalCore)
add_test(NAME GpuCompatibilityTest COMMAND test_gpu_compatibility)

add_executable(test_authoritative_session tests/unit/test_authoritative_session.cpp)
target_link_libraries(test_authoritative_session PRIVATE LibertyUniversalCore)
add_test(NAME AuthoritativeSessionTest COMMAND test_authoritative_session)

add_executable(test_media_id_validator tests/unit/test_media_id_validator.cpp)
target_link_libraries(test_media_id_validator PRIVATE LibertyUniversalCore)
add_test(NAME MediaIdValidatorTest COMMAND test_media_id_validator)

add_executable(run_all_tests tests/unit/run_all_tests.cpp)
target_link_libraries(run_all_tests PRIVATE LibertyUniversalCore)
add_test(NAME AllSubsystemsTest COMMAND run_all_tests)
