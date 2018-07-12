

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "config.h"
#include "lib/tcpsock.h"
#include <poll.h>
#include <unistd.h>
#include "lib/dplist.h"

#ifndef TIMEOUT
  #define TIMEOUT 5
#endif
#ifndef MAX_CONN
  #define MAX_CONN 5
#endif

void connmgr_free();
void connmgr_listen(int port_number);
