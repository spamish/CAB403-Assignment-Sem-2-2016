/*  File Name: Client.h
 *  Unit Code: CAB403
 *  @author Samuel Janetzki (n7402643)
 *  @version v0.0
 *  @brief Description
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/wait.h>

#define BUFFSIZE        100
#define BUFFINIT        101
#define ERROR           -1
#define TRUE            1
#define FALSE           0

/** @brief Description
 *  @param variable_name Description of variable
 *  @return Description of return
 */
int auto_mach_tell(int id, char *buff);