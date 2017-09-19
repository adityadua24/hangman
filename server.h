#ifndef SERVER_HEAD
#define SERVER_HEAD

#include <stdio.h>      /* Input/Output */
#include <stdlib.h>     /* General Utilities */
#include <string.h>     /* String handling */
#include <unistd.h>     /* Symbolic Constants */
#include <sys/types.h>  /* Primitive System Data Types */
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <arpa/inet.h>
#include <assert.h>
#include <pthread.h>    /* POSIX Threads */
#include <errno.h>      /* Errors */
#include <omp.h>        /* OMP API */
#include "reader.h"     /* Txt file Reader */
#include "connection.h" /* server-client connection */
#include "game.h"
#include "sys_ops.h"

#endif