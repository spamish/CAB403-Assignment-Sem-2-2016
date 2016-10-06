/*  File Name: Server.h
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
#define ERROR           -1
#define TRUE            1
#define FALSE           0
#define FIN             0

#define USERNAME        '1'
#define PASSWORD        '2'
#define ACCOUNTS        '3'
#define TRANSFER        '4'
#define BALANCE         '5'
#define HISTORY         '6'
#define FINISH          '7'
#define EXIT            '8'

/** @brief Loop to read and handle client actions
 *  @param id Client ID
 *  @return Continuation status
 */
int client_actions(int id);

/** @brief Interrupt handler for CTRL C
 *  @param dummy Dummy variable.
 *  @return void
 */
void interrupt(int dummy);