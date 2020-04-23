#if defined(__clang__)
  #pragma clang diagnostic ignored "-Wparentheses"
#endif

#ifdef __GENODE__
void *genode_alloc_secondary_stack(unsigned long stack_size);
void genode_free_secondary_stack(void *stack);
#endif

#if defined(__clang__) || defined(__GNUC__)
  #if defined(__i386__)
    #include "x86.c"
  #elif defined(__amd64__)
    #include "amd64.c"
  #elif defined(__psp__)
    #include "psp1.c"
  #elif defined(__vita__)
    #include "scefiber.c"
  #elif defined(__arm__)
    #include "arm.c"
  #elif defined(__aarch64__)
    #include "aarch64.c"
  #elif defined(__powerpc64__) && defined(_CALL_ELF) && _CALL_ELF == 2
    #include "ppc64v2.c"
  #elif defined(_ARCH_PPC) && !defined(__LITTLE_ENDIAN__)
    #include "ppc.c"
  #elif defined(_WIN32)
    #include "fiber.c"
  #else
    #include "sjlj.c"
  #endif
#elif defined(_MSC_VER)
  #include <Windows.h>
  #if WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    #include "fiber.c"
  #elif defined(_M_IX86)
    #include "x86.c"
  #elif defined(_M_AMD64)
    #include "amd64.c"
  #else
    #include "fiber.c"
  #endif
#else
  #error "libco: unsupported processor, compiler or operating system"
#endif
