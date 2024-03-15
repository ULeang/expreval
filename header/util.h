#ifndef __UTIL_H__
#define __UTIL_H__

#include <stdio.h>

#define fatal(exit_code, fmt, args...)                          \
  ({                                                            \
    fprintf(stderr, "\033[1;31mfatal\033[m:" fmt "\n", ##args); \
    exit(exit_code);                                            \
  })
#define usage(exit_code, fmt, args...)                          \
  ({                                                            \
    fprintf(stderr, "\033[1;34musage\033[m:" fmt "\n", ##args); \
    exit(exit_code);                                            \
  })
#define warn(fmt, args...) \
  ({ fprintf(stderr, "\033[1;31merror\033[m:" fmt "\n", ##args); })

typedef enum ExitCode {
  SUCCESS        = 0,
  NUMPREFIXERR   = 1,
  NUMERR         = 2,
  UNRECOGCHARERR = 3,
  FREESTACKERR   = 4,
  PARSEERR       = 5,
  EVALERR        = 6,
} ExitCode;

#endif
