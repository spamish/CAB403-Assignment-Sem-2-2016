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
#include <math.h>

#define DEFAULT_PORT    12345
#define MAX_LIMIT       5000.0
#define MAX_DEPOSIT     1000.0

#define BACKLOG         1
#define BUFFSIZE        100
#define INPUTSIZE       20
#define PAD             '0'
#define CANCEL          'c'
#define ERROR           -1
#define TRUE            1
#define FALSE           0
#define FIN             -2
#define ARGS            10
#define FAIL            1

#define LOGIN           1
#define ACCOUNTS        2
#define TRANSFER        3
#define HISTORY         4
#define EXTERNAL        5
#define EXIT            6

#define SAVE            11
#define LOAN            12
#define CRED            13
#define SCALE           11

#define BALA            '1'
#define DEPO            '2'
#define WITH            '3'
#define TRAN            '4'
#define HIST            '5'

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
    char type;
    double amount;
} DB_TRANSACTION;
