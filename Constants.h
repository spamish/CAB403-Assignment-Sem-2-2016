/*  File Name: Constants.h
 *  Unit Code: CAB403
 *  @author Samuel Janetzki (n7402643)
 *  @version v0.0
 *  @brief Description
 */

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define DEFAULT_PORT    12345
#define BACKLOG         1
#define BUFFSIZE        100
#define BUFFINIT        101
#define INPUTSIZE       12
#define PAD             '0'
#define ERROR           -1
#define TRUE            1
#define FALSE           0
#define FIN             0
#define CANCEL          c

#define LOGIN           1
#define ACCOUNTS        2
#define TRANSFER        3
#define BALANCE         4
#define HISTORY         5
#define EXIT            6
#define FINISH          7
