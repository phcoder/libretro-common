/*
  libco.mips (2020-04-20)
  author: Google
  license: MIT
*/

#define LIBCO_C
#include <libco.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#ifndef __APPLE__
#include <malloc.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

static thread_local uint32_t co_active_buffer[64];
static thread_local cothread_t co_active_handle;

__asm__ (
      ".align 4\n"
      ".globl co_switch_mips\n"
      ".globl _co_switch_mips\n"
      "co_switch_mips:\n"
      "_co_switch_mips:\n"
      " sd $s0, 0($a1)\n"
      " sd $s1, 8($a1)\n"
      " sd $s2, 0x10($a1)\n"
      " sd $s3, 0x18($a1)\n"
      " sd $s4, 0x20($a1)\n"
      " sd $s5, 0x28($a1)\n"
      " sd $s6, 0x30($a1)\n"
      " sd $s7, 0x38($a1)\n"
      " sd $gp, 0x40($a1)\n"
      " sd $sp, 0x48($a1)\n"
      " sd $fp, 0x50($a1)\n"
      " sd $ra, 0x58($a1)\n"
      " ld $s0, 0($a0)\n"
      " ld $s1, 8($a0)\n"
      " ld $s2, 0x10($a0)\n"
      " ld $s3, 0x18($a0)\n"
      " ld $s4, 0x20($a0)\n"
      " ld $s5, 0x28($a0)\n"
      " ld $s6, 0x30($a0)\n"
      " ld $s7, 0x38($a0)\n"
      " ld $gp, 0x40($a0)\n"
      " ld $sp, 0x48($a0)\n"
      " ld $fp, 0x50($a0)\n"
      " ld $ra, 0x58($a0)\n"
      " jr $ra\n"
      "  nop\n"
      ".align 4\n"
      ".globl store_gp\n"
      ".globl _store_gp\n"
      "store_gp:\n"
      "_store_gp:\n"
      " sd $gp, 0($a0)\n"
      " jr $ra\n"
      "  nop\n"      
    );

/* ASM */
void co_switch_mips(cothread_t handle, cothread_t current);
void store_gp(uint64_t *s);

cothread_t co_create(unsigned int size, void (*entrypoint)(void))
{
   size = (size + 1023) & ~1023;
   cothread_t handle = 0;
#if defined(__APPLE__) || HAVE_POSIX_MEMALIGN >= 1
   if (posix_memalign(&handle, 1024, size + 512) < 0)
      return 0;
#else
   handle = memalign(1024, size + 512);
#endif

   if (!handle)
      return handle;

   uint64_t *ptr = (uint64_t*)handle;
   memset(ptr, 0, 512);
   /* Non-volatiles.  */
   /* ptr[0],..., ptr[7] -> s0,..., s7 */
   store_gp(&ptr[8]); /* gp */
   ptr[9] = (uintptr_t)ptr + size + 512 - 8; /* sp  */
   /* ptr[10] is fp */
   ptr[11] = (uintptr_t)entrypoint; /* ra */
   return handle;
}

cothread_t co_active(void)
{
   if (!co_active_handle)
      co_active_handle = co_active_buffer;
   return co_active_handle;
}

void co_delete(cothread_t handle)
{
   free(handle);
}

void co_switch(cothread_t handle)
{
   cothread_t co_previous_handle = co_active();
   co_switch_mips(co_active_handle = handle, co_previous_handle);
}

#ifdef __cplusplus
}
#endif
