#include "gd32vf103_usart.h"
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

// #define UART_AUTO_ECHO

ssize_t _read(int fd, void *ptr, size_t len) { return -1; }
