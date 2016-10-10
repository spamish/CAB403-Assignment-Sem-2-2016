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
#define INPUTSIZE       20
#define PAD             '0'
#define ERROR           -1
#define TRUE            1
#define FALSE           0
#define FIN             0
#define CANCEL          c
#define ARGS            10

#define LOGIN           1
#define ACCOUNTS        2
#define TRANSFER        3
#define BALANCE         4
#define HISTORY         5
#define EXIT            6
#define FINISH          7

/* @brief authentication struct
 */
typedef struct
{
    char username[21];
    char password[11];
    char client[9];
} DB_AUTHENTICATON;

/* @brief account struct
 */
typedef struct
{
    char number[9];
    double opening;
    double closing;
} DB_ACCOUNT;

/* @brief client struct
 */
typedef struct
{
    char firstname[21];
    char lastname[21];
    char client[9];
    char accounts[3][9];
} DB_CLIENT;

/* @brief transaction struct
 */
typedef struct
{
    char from[9];
    char to[9];
    char trantype[2];
    double amount;
} DB_TRANSACTION;
