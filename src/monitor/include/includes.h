#define _XOPEN_SOURCE
#define _SVID_SOURCE || _BSD_SOURCE || _XOPEN_SOURCE >= 500

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <signal.h>
#include <getopt.h>
#include <syslog.h>
#include <sys/select.h>
#include <dlfcn.h>
#include <talloc.h>
#include "../../../iniparser3.0b/src/iniparser.h"
#include "../../../include/common.h"
#include "../../../include/version.h"
#include "monitor-fn-enum.h"
#include "configuration.h"
#include "network.h"

