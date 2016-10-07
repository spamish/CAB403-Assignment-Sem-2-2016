#include "Server.h"

/* sock         - id of server socket
 * conn         - id of client socket
 */
int sock, conn = 0;

int main(int argc, char *argv[])
{
    /* port         - port address to use
     * server_addr  - endpoint for server
     * client_addr  - endpoint for client
     * sock_size    - data size of socket information
     * buffer       - buffer to place text data in
     */
    int port = DEFAULT_PORT;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sock_size;
    char buffer[BUFFSIZE];
    
    // @todo read in files and set up databases
    
    // check if port number is given.
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    
    // setup interrupt handler
    signal(SIGINT, interrupt);
    
    // make the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR)
    {
        perror("Problem setting up socket");
        exit(1);
    }
    
    // set the endpoint
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    // bind socket to the endpoint
    if (bind(sock, (struct sockaddr *) &server_addr, \
            sizeof(struct sockaddr)) == ERROR)
    {
        perror("Problem binding socket");
        exit(1);
    }
    
    // start listening
    if (listen(sock, BACKLOG) == ERROR)
    {
        perror("Problem listening");
        exit(1);
    }
    
    printf("Server Listening\n");
    
    // run indefinitely
    while (TRUE)
    {
        sock_size = sizeof(struct sockaddr_in);
        
        // accept connection
        if ((conn = accept(sock, (struct sockaddr *) &client_addr, \
                &sock_size)) == ERROR)
        {
            perror("Problem accepting connection");
            continue;
        }
        
        sprintf(buffer, "%d", conn);
        
        // send id to user for future expansion and confirmation
        if (send(conn, buffer, strlen(buffer), 0) == ERROR)
        {
            perror("Problem sending id");
            close(conn);
            continue;
        }
        
        printf("Client connected, id: %d\n", conn);
        
        // client connected and doing shit
        while(client_actions(conn)) {}
        
        // close client connection
        close(conn);
        conn = 0;
    }
}

int client_actions(int id)
{
    int num;
    char rec[BUFFINIT];
    char sen[BUFFINIT];
    // BUFFERS!
    memset(rec, 0, BUFFINIT);
    memset(sen, 0, BUFFINIT);
    
    printf("Receive instruction\n");
    
    // retreive client instruction
    if ((num = read(id, rec, BUFFSIZE)) > FIN)
    {
        rec[BUFFSIZE] = '\0';
    }
    
    if (num <= FIN)
    {
        printf("Problem receiving instruction");
        return FALSE;
    }
    
    printf("Handle instruction\n");
    
    // command goes off for processing
    handler(sen, rec);
    
    // send response (might change to within handler)
    if (send(id, sen, strlen(sen), 0) == ERROR)
    {
        perror("Problem sending response");
        return FALSE;
    }
    
    printf("mess: %s, siz: %d\n", sen, strlen(sen));
    
    // nothing fucked up, wait for more from client
    return TRUE;
}

void handler(char *sen, char *rec)
{
    size_t siz;
    int prog = 0;
    // @todo set to maximum number of arguments
    char **arguments = malloc(sizeof(*arguments)*2);
    
    switch (rec[prog++] - PAD)
    {
        case LOGIN:
            // @param username password
            // @return accountno firstname lastname
            // @fail exit
            
            // retrieve username
            siz = (rec[prog++] - PAD) * sizeof(char);
            arguments[0] = (char *) malloc(siz);
            memcpy(arguments[0], &rec[prog], siz);
            prog += siz;
            
            // retrieve password
            siz = (rec[prog++] - PAD) * sizeof(char);
            arguments[1] = (char *) malloc(siz);
            memcpy(arguments[1], &rec[prog], siz);
            
            // @todo check username and password
            
            // @todo return login for pass or exit for fail
            
            sprintf(sen, "%c", LOGIN + PAD);
            break;
        
        case ACCOUNTS:
            // @param accountno
            // @return array[accountno]
            
            // @todo check client accounts
            sprintf(sen, "%c", ACCOUNTS + PAD);
            break;
        
        case TRANSFER:
            // @param 
            // @return 
            
            // @todo check account number
            sprintf(sen, "%c", TRANSFER + PAD);
            break;
        
        case BALANCE:
            // @param accountno
            // @return openingbal closingbal
            
            sprintf(sen, "%c", BALANCE + PAD);
            break;
        
        case HISTORY:
            // @param accountno
            // @return array[fromaccount toaccount trantype amount]
            
            sprintf(sen, "%c", HISTORY + PAD);
            break;
        
        case EXIT:
            // @param exit
            // @return exit
            
            sprintf(sen, "%c", EXIT + PAD);
            break;
        
        case FINISH:
            // @param 
            // @return finish
            
            // @todo move transaction from temporary to permanent data
            sprintf(sen, "%c", FINISH + PAD);
            break;
    }
}

void interrupt(int dummy)
{
    printf("Shutdown initiated\n");
    
    // stop receiving on socket
    if (shutdown(sock, 1) == ERROR)
    {
        perror("Problem shutting down socket");
        exit(1);
    }
    
    printf("Closing connections\n");
    
    // close connections to clients
    if (conn)
    {
        // @todo send closing notification to clients
        
        if (close(conn) == ERROR)
        {
            perror("Problem closing socket");
            exit(1);
        }
    }
    
    // close socket
    if (close(sock) == ERROR)
    {
        perror("Problem closing socket");
        exit(1);
    }
    
    //printf("Saving data\n");
    
    // @todo update data in text files
    
    printf("Exiting program\n");
    exit(0);
}
