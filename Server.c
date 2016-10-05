#include "Server.h"

int sock;

int main(int argc, char *argv[])
{
    /* sock         - id of server socket
     * conn         - id of client socket
     * port         - port address to use
     * server_addr  - endpoint for server
     * client_addr  - endpoint for client
     * sock_size    - data size of socket information
     * buffer       - buffer to place text data in
     */
    int conn, port = DEFAULT_PORT;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sock_size;
    char buffer[BUFFSIZE];
    
    // check if port number is given.
    if (argc == 2) {
        port = atoi(argv[1]);
    }
    
    // setup interrupt handler
    signal(SIGINT, interrupt);
    
    // make the socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == ERROR) {
        perror("Problem setting up socket");
        exit(1);
    }
    
    printf("Socket created\n");
    
    // set the endpoint
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    printf("Endpoint set\n");
    
    // bind socket to the endpoint
    if (bind(sock, (struct sockaddr *) &server_addr, \
            sizeof(struct sockaddr)) == ERROR) {
        perror("Problem binding socket");
        exit(1);
    }
    
    printf("Socket bound\n");
    
    // start listening
    if (listen(sock, BACKLOG) == ERROR) {
        perror("Problem listening");
        exit(1);
    }
    
    printf("Server Listening\n");
    
    // run indefinitely
    while (TRUE) {
        sock_size = sizeof(struct sockaddr_in);
        
        // accept connection
        if ((conn = accept(sock, (struct sockaddr *) &client_addr, \
                &sock_size)) == ERROR)
        {
            perror("Problem accepting connection");
            continue;
        }
        
        printf("Client connected to server\n");
        sprintf(buffer, "%d", conn);
        if (send(conn, &buffer, sizeof(buffer), 0) == ERROR) {
            perror("Problem sending id");
            close(conn);
            continue;
        }
        
        // handle client actions while connected
        while(client_actions(conn)) {}
        
        // close client connection
        close(conn);
    }
}

int client_actions(int id) {
    //action = read(reply)
    
    /*switch(reply)
    {
        case username
            Check if exists
            send(Pass or fail)
            break;
        
        case password
            Check if correct
            send(Pass or fail)
            break;
        
        case accounts
            Check client accounts
            send(existing accounts)
            break;
        
        case transfer
            Check account number
            send(Pass or fail)
            break;
        
        case balance
            send(account balance)
            break;
        
        case history
            send(transaction listings)
            break;
        
        case finish
            Move transaction from temporary to permanent data
            break;
        
        case exit
            Clear temporary data
            break;
        
        default
            send(error)
            break;
    }*/
    
    return FALSE;
}

void interrupt(int dummy) {
    printf("Shutdown initiated\n");
    
    if (shutdown(sock, 1) == ERROR) {
        perror("Problem shutting down socket");
        exit(1);
    }
    
    printf("Closing connections\n");
    
    if (close(sock) == ERROR) {
        perror("Problem closing socket");
        exit(1);
    }
    
    //printf("Saving data\n");
    
    printf("Exiting program\n");
    exit(0);
}
