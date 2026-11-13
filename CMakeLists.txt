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
    LibertyRecomp/diagnostics/resource_budget.cpp
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

add_executable(test_resource_budget tests/unit/test_resource_budget.cpp)
target_link_libraries(test_resource_budget PRIVATE LibertyUniversalCore)
add_test(NAME ResourceBudgetTest COMMAND test_resource_budget)

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


# Optional bridge to the pinned, full LibertyRecomp/Theft4 iOS runtime source.
# It is deliberately opt-in: the Universal host/Android core remains buildable
# without Apple's SDK or the iOS-only CMake 3.29/C++23 runtime toolchain.
option(UNIVERSAL_ENABLE_THEFT4_IOS_RUNTIME
    "Expose build targets for the pinned Theft4 iOS runtime submodule" OFF)

if(UNIVERSAL_ENABLE_THEFT4_IOS_RUNTIME)
    set(UNIVERSAL_THEFT4_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/theft4")

    if(NOT EXISTS "${UNIVERSAL_THEFT4_SOURCE_DIR}/CMakePresets.json")
        message(FATAL_ERROR
            "The pinned Theft4 runtime is missing. Initialize submodule runtime/theft4.")
    endif()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_custom_target(universal_ios_device_runtime_core
        COMMAND "${Python3_EXECUTABLE}" tools/setup_repo.py
        COMMAND "${CMAKE_COMMAND}" --preset ios-device-debug
        COMMAND "${CMAKE_COMMAND}" --build --preset ios-device-debug --parallel 3
        WORKING_DIRECTORY "${UNIVERSAL_THEFT4_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "Build pinned Theft4 iPhoneOS ARM64 runtime core/link probe")

    add_custom_target(universal_ios_simulator_runtime_core
        COMMAND "${Python3_EXECUTABLE}" tools/setup_repo.py
        COMMAND "${CMAKE_COMMAND}" --preset ios-simulator-debug
        COMMAND "${CMAKE_COMMAND}" --build --preset ios-simulator-debug --parallel 3
        WORKING_DIRECTORY "${UNIVERSAL_THEFT4_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "Build pinned Theft4 iOS Simulator ARM64 runtime core/link probe")
endif()


# Optional bridge to the pinned Android LibertyRecomp runtime.
option(UNIVERSAL_ENABLE_XHYN_ANDROID_RUNTIME
    "Expose build targets for the pinned XHYN Android runtime submodule" OFF)

if(UNIVERSAL_ENABLE_XHYN_ANDROID_RUNTIME)
    set(UNIVERSAL_XHYN_SOURCE_DIR
        "${CMAKE_CURRENT_SOURCE_DIR}/runtime/xhyn-android")

    if(NOT EXISTS "${UNIVERSAL_XHYN_SOURCE_DIR}/os/android/scripts/build_native.sh")
        message(FATAL_ERROR
            "The pinned XHYN Android runtime is missing. Initialize submodule runtime/xhyn-android.")
    endif()

    find_package(Python3 REQUIRED COMPONENTS Interpreter)

    add_custom_target(universal_android_arm64_runtime
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/apply_android_low_end_profile.py" "${UNIVERSAL_XHYN_SOURCE_DIR}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/apply_mali_decode_pool.py" "${UNIVERSAL_XHYN_SOURCE_DIR}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/apply_android_audio_diagnostics.py" "${UNIVERSAL_XHYN_SOURCE_DIR}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/apply_universal_android_branding.py" "${UNIVERSAL_XHYN_SOURCE_DIR}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/apply_native_tombstone_capture.py" "${UNIVERSAL_XHYN_SOURCE_DIR}"
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/tools/apply_public_game_storage.py" "${UNIVERSAL_XHYN_SOURCE_DIR}"
        COMMAND "${CMAKE_COMMAND}" -E env
            "ANDROID_NDK=$ENV{ANDROID_NDK}"
            "BUILD_TYPE=Release"
            "STRIP=1"
            bash os/android/scripts/build_native.sh 2
        COMMAND "${CMAKE_COMMAND}" -E env
            "ANDROID_NDK=$ENV{ANDROID_NDK}"
            bash os/android/scripts/build_driver_proxy.sh
        WORKING_DIRECTORY "${UNIVERSAL_XHYN_SOURCE_DIR}"
        USES_TERMINAL
        COMMENT "Build optimized pinned XHYN ARM64 runtime and Android driver bridge")

    add_custom_target(universal_android_arm64_apk
        COMMAND bash os/android/scripts/build_apk.sh debug
        COMMAND bash os/android/scripts/build_apk.sh release
        WORKING_DIRECTORY "${UNIVERSAL_XHYN_SOURCE_DIR}"
        DEPENDS universal_android_arm64_runtime
        USES_TERMINAL
        COMMENT "Package installable debug and release GTA IV Universal APKs")
endif()
