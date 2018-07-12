
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "config.h"
#include "tcpsock.h"
#include <poll.h>
#include <unistd.h>
#include "dplist.h"
#include "connmgr.h"

int main(void)
{
connmgr_listen(5678);
connmgr_free();
return 0;
}
