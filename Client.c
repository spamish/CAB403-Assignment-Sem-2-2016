#include "Client.h"
#include "Textable.h"

int main(int argc, char *argv[])
{
    /* sock         - id of socket
     * port         - port address to use
     * conn_addr    - endpoint for connection
     * ip_address   - ip address of server
     * buffer       - buffer to place text data in
     */
    int conn, id, port, numbytes = 0;
    struct sockaddr_in conn_addr;
    struct hostent *ip_address;
    char message[BUFFSIZE];
    char reply[BUFFSIZE];
    
    // check input parameters
    if (argc != 3)
    {
        perror("Not enough input arguments");
        exit(1);
    }
    
    // get the ip address
    if ((ip_address = gethostbyname(argv[1])) == NULL)
    {
        perror("Problem translating ip address");
        exit(1);
    }
    
    // make the socket
    if ((conn = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("Problem setting up socket");
        exit(1);
    }
    
    // set the endpoint
	conn_addr.sin_family = AF_INET;
	conn_addr.sin_port = htons(atoi(argv[2]));
	conn_addr.sin_addr = *((struct in_addr *) ip_address->h_addr);
    
    // connect to server
    if (connect(conn, (struct sockaddr *)&conn_addr, \
            sizeof(struct sockaddr)) == ERROR)
    {
        perror("Problem connecting to server");
        exit(1);
    }
    
    // get id
    if ((numbytes = read(conn, reply, BUFFSIZE)) > FIN)
    {
        reply[BUFFSIZE] = '\0';
    }
    
    if (numbytes <= FIN)
    {
        printf("Problem receiving id\n");
        exit(1);
    }
    
    // communication confirmed
    id = atoi(reply);
    printf("Connected to server with id: %d\n", id);
    
    // welcome screen and login
    if (welcome_login(conn))
    {
        printf(LOGIN_FAIL);
        exit(1);
    }
    
    // logged in and doing shit
    while (auto_mach_tell(conn)) {}
}

int welcome_login(int sock)
{
    // MOAR BUFFERS!
    char message[BUFFSIZE];
    char reply[BUFFSIZE];
    char username[INPUTSIZE];
    char password[INPUTSIZE];
    
    memset(message, 0, BUFFINIT);
    memset(reply, 0, BUFFINIT);
    
    // enter username
    printf(LOGIN_WELCOME);
    scanf("%s", &username);
    
    // enter password
    printf(LOGIN_PASSWORD);
    scanf("%s", &password);
    
    // compile message
    sprintf(message, "%c%c%s%c%s", LOGIN + PAD, \
            strlen(username) + PAD, username, \
            strlen(password) + PAD, password);
    
    // communicate with server
    if(talking(sock, message, reply) == ERROR)
    {
        printf("Problem logging in\n");
        exit(1);
    }
    
    // check if pass or fail received
    sprintf(message, "%c", EXIT);
    return !strcmp(reply, message);
}

int talking(int sock, char *sen, char *rec)
{
    int num;
    
    // send message
    if (send(sock, sen, strlen(sen), 0) == ERROR)
    {
        perror("Problem sending response");
        return FALSE;
    }
    
    printf("mess: %s, siz: %d\n", sen, strlen(sen));
    
    // read response
    if ((num = read(sock, rec, BUFFSIZE)) > FIN)
    {
        rec[BUFFSIZE] = '\0';
        printf("rec: %s, siz: %d\n", rec, strlen(rec));
    }
    
    // check if response is bullshit
    if (num <= FIN)
    {
        printf("Problem receiving response");
        return FALSE;
    }
    
    return TRUE;
}

int auto_mach_tell(int sock)
{
    // @todo display main page
    
    // @todo request selection
    
    /* @todo switch(selection)
    {
        case tranfer:
            //status = with_depo_tran(params)
            break
        
        case accounts:
            //status = bala_hist(params)
            break
        
        case exit:
            //return exit
        
        default:
            printf(FAIL)
    }*/
    
    return TRUE;
}

/* @todo return with_depo_tran(params) {
    Display accounts
    
    Pass or fail
    
    if (transaction) {
        Display accounts
        
        Pass or fail
    }
    
    Request amount
    
    Pass or fail
    
    Display result
    
    return
} */

/* @todo return bala_hist(params) {
    Display accounts
    
    if (accounts) {
        Display balance
    } else { // History
        Display transaction listings
    }
    
    return
} */
