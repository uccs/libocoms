/* confdefs.h */
#define PACKAGE_NAME "Common Communication Substrate"
#define PACKAGE_TARNAME "ccs"
#define PACKAGE_VERSION "0.1"
#define PACKAGE_STRING "Common Communication Substrate 0.1"
#define PACKAGE_BUGREPORT ""
#define PACKAGE_URL ""
#define CCS_ARCH "x86_64-apple-darwin10.7.0"
#define STDC_HEADERS 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STRINGS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define __EXTENSIONS__ 1
#define _ALL_SOURCE 1
#define _GNU_SOURCE 1
#define _POSIX_PTHREAD_SEMANTICS 1
#define _TANDEM_SOURCE 1
#define ORTE_ENABLE_PROGRESS_THREADS 0
#define CCS_ENABLE_PROGRESS_THREADS 0
#define CCS_ENABLE_MEM_DEBUG 0
#define CCS_ENABLE_MEM_PROFILE 0
#define CCS_ENABLE_DEBUG 0
#define CCS_WANT_PRETTY_PRINT_STACKTRACE 1
#define CCS_ENABLE_PTY_SUPPORT 1
#define CCS_ENABLE_HETEROGENEOUS_SUPPORT 0
#define CCS_ENABLE_TRACE 0
#define CCS_ENABLE_FT 0
#define CCS_ENABLE_FT_CR 0
#define CCS_WANT_HOME_CONFIG_FILES 1
#define CCS_ENABLE_IPV6 0
#define CCS_PACKAGE_STRING "Open MPI rg6@grahammbpro.local Distribution"
#define CCS_IDENT_STRING ""
#define CCS_MAX_PROCESSOR_NAME 256
#define CCS_MAX_ERROR_STRING 256
#define CCS_MAX_OBJECT_NAME 64
#define CCS_MAX_INFO_KEY 36
#define CCS_MAX_INFO_VAL 256
#define CCS_MAX_PORT_NAME 1024
#define CCS_MAX_DATAREP_STRING 128
#define CCS_ENABLE_CRDEBUG 0
#define CCS_CUDA_SUPPORT 0
#define CCS_CC "gcc"
#define STDC_HEADERS 1
#define _GNU_SOURCE 1
#define CCS_C_HAVE_BUILTIN_EXPECT 1
#define CCS_C_HAVE_BUILTIN_PREFETCH 1
#define CCS_CC_USE_PRAGMA_IDENT 0
#define CCS_CC_USE_IDENT 0
#define CCS_CC_USE_PRAGMA_COMMENT 
#define CCS_CC_USE_CONST_CHAR_IDENT 1
#define HAVE_INT8_T 1
#define HAVE_UINT8_T 1
#define HAVE_INT16_T 1
#define HAVE_UINT16_T 1
#define HAVE_INT32_T 1
#define HAVE_UINT32_T 1
#define HAVE_INT64_T 1
#define HAVE_UINT64_T 1
#define HAVE_LONG_LONG 1
#define HAVE_LONG_DOUBLE 1
#define HAVE_FLOAT__COMPLEX 1
#define HAVE_DOUBLE__COMPLEX 1
#define HAVE_LONG_DOUBLE__COMPLEX 1
#define HAVE_INTPTR_T 1
#define HAVE_UINTPTR_T 1
#define HAVE_MODE_T 1
#define HAVE_SSIZE_T 1
#define HAVE_PTRDIFF_T 1
#define SIZEOF_CHAR 1
#define SIZEOF_SHORT 2
#define SIZEOF_INT 4
#define SIZEOF_LONG 8
#define SIZEOF_LONG_LONG 8
#define SIZEOF_FLOAT 4
#define SIZEOF_DOUBLE 8
#define SIZEOF_LONG_DOUBLE 16
#define SIZEOF_FLOAT__COMPLEX 8
#define SIZEOF_DOUBLE__COMPLEX 16
#define SIZEOF_LONG_DOUBLE__COMPLEX 32
#define SIZEOF_VOID_P 8
#define SIZEOF_SIZE_T 8
#define SIZEOF_SSIZE_T 8
#define SIZEOF_PTRDIFF_T 8
#define SIZEOF_WCHAR_T 4
#define CCS_ALIGNMENT_BOOL 1
#define CCS_ALIGNMENT_INT8 1
#define CCS_ALIGNMENT_INT16 2
#define CCS_ALIGNMENT_INT32 4
#define CCS_ALIGNMENT_INT64 8
#define CCS_ALIGNMENT_CHAR 1
#define CCS_ALIGNMENT_SHORT 2
#define CCS_ALIGNMENT_WCHAR 4
#define CCS_ALIGNMENT_INT 4
#define CCS_ALIGNMENT_LONG 8
#define CCS_ALIGNMENT_LONG_LONG 8
#define CCS_ALIGNMENT_FLOAT 4
#define CCS_ALIGNMENT_DOUBLE 8
#define CCS_ALIGNMENT_LONG_DOUBLE 16
#define CCS_ALIGNMENT_FLOAT_COMPLEX 4
#define CCS_ALIGNMENT_DOUBLE_COMPLEX 8
#define CCS_ALIGNMENT_LONG_DOUBLE_COMPLEX 16
#define CCS_ALIGNMENT_VOID_P 8
/* end confdefs.h.  */

#include <stdio.h>
#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
# include <sys/stat.h>
#endif
#ifdef STDC_HEADERS
# include <stdlib.h>
# include <stddef.h>
#else
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
#endif
#ifdef HAVE_STRING_H
# if !defined STDC_HEADERS && defined HAVE_MEMORY_H
#  include <memory.h>
# endif
# include <string.h>
#endif
#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif
#ifdef HAVE_INTTYPES_H
# include <inttypes.h>
#endif
#ifdef HAVE_STDINT_H
# include <stdint.h>
#endif
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
int
main ()
{
bool bar, foo = true; bar = foo;
  ;
  return 0;
}
