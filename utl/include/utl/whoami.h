#pragma once

#if defined(_MSC_VER)
#define COMPILER_MSVC 1
#define COMPILER_GCC 0
#define COMPILER_CLANG 0
#elif defined(__GNUC__)
#define COMPILER_MSVC 0
#define COMPILER_GCC 1
#define COMPILER_CLANG 0
#elif defined(__clang__)
#define COMPILER_MSVC 0
#define COMPILER_GCC 0
#define COMPILER_CLANG 1
#else
#error "Unsupported compiler"
#endif

#if COMPILER_MSVC
#if defined(_M_X64)
#define ARCH_X86_64 1
#define ARCH_ARM64 0
#elif defined(_M_ARM64)
#define ARCH_X86_64 0
#define ARCH_ARM64 1
#else
#error "Unsupported architecture"
#endif
#elif COMPILER_GCC || COMPILER_CLANG
#if defined(__x86_64__)
#define ARCH_X86_64 1
#define ARCH_ARM64 0
#elif defined(__aarch64__)
#define ARCH_X86_64 0
#define ARCH_ARM64 1
#else
#error "Unsupported architecture"
#endif
#endif

#if defined(_WIN32)
#define OS_WINDOWS 1
#define OS_LINUX 0
#define OS_MACOSX 0
#elif defined(__linux__)
#define OS_WINDOWS 0
#define OS_LINUX 1
#define OS_MACOSX 0
#elif defined(__APPLE__)
#define OS_WINDOWS 0
#define OS_LINUX 0
#define OS_MACOSX 1
#else
#error "Unsupported OS"
#endif

#if !defined(BUILD_DEBUG)
#define BUILD_DEBUG 1
#endif