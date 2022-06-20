#ifndef _DEF_H_
#define _DEF_H_

#ifdef __linux__
#define __ubuntu__
#endif

#ifdef __DOS__
#define __dos__
#endif 

#ifdef __ubuntu__
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pci/pci.h"
#include "pciaccess.h"
#endif

#ifdef __dos__
#include <i86.h>
#include <dos.h>
#include <bios.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <conio.h>
#endif

#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <memory.h>


#include "common.h"
#include "md5.h"
#include "main.h"
#include "pcie.h"
#include "flash.h"
#include "share.h"
#include "cmd.h"
#include "clk.h"
#include "ts.h"
#include "spi_flash.h"
#include "memtest.h"
#include "ini.h"
#include "i2c.h"

#endif
