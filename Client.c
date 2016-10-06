#include "Client.h"

/*Pass or fail
==
Pass {
    cont
} else {
    return error;
}*/

int main(int argc, char *argv[])
{
    /* sock         - id of socket
     * port         - port address to use
     * conn_addr    - endpoint for connection
     * ip_address   - ip address of server
     * buffer       - buffer to place text data in
     */
    int conn, port, numbytes = 0;
    struct sockaddr_in conn_addr;
    struct hostent *ip_address;
    char buffer[BUFFSIZE];
    
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
    
    printf("Socket created\n");
    
    // set the endpoint
	conn_addr.sin_family = AF_INET;
	conn_addr.sin_port = htons(atoi(argv[2]));
	conn_addr.sin_addr = *((struct in_addr *) ip_address->h_addr);
    
    printf("Endpoint set\n");
    
    if (connect(conn, (struct sockaddr *)&conn_addr, \
            sizeof(struct sockaddr)) == ERROR)
    {
        perror("Problem connecting to server");
        exit(1);
    }
    
    memset(&buffer, '0', BUFFINIT);
    printf("Connected to server, buffer size: %d\n", strlen(buffer));
    
    if ((numbytes = read(conn, buffer, BUFFSIZE)) == ERROR)
    {
		perror("Problem receiving message");
		exit(1);
	}
    
    buffer[numbytes] = '\0';
    printf("ID confirmed as %s", buffer);
    
    while (auto_mach_tell(conn, buffer)) {}
}

int auto_mach_tell(int id, char *buff)
{
    int num = 0;
    
    printf("\nInput please? ");
    scanf("%s", buff);
    printf("%s\n", buff);
    
    if (send(id, buff, strlen(buff), 0) == ERROR)
    {
        perror("Problem sending instruction");
    }
    
    printf("Data sent: %s\n", buff);
    memset(&buff, '0', BUFFINIT);
    
    if ((num = read(id, &buff, BUFFSIZE)) == ERROR)
    {
		perror("Problem receiving response");
	}
    
    buff[num] = '\0';
    printf("size: %d, message: %s\n", num, buff);
    
    return TRUE;
}
    
    /*Display welcome message
    
    Request username
    
    Pass or fail
    
    Request password
    
    Pass or fail
    
    while (true) {
        Display main page
        
        Request selection
        
        switch(selection)
        {
            case tranfer
                status = with_depo_tran(params)
                break
            
            case accounts
                status = bala_hist(params)
                break
            
            case exit
                return exit
            
            case default
                Invalid option selected
        }
    }*/

/* return with_depo_tran(params) {
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

/* return bala_hist(params) {
    Display accounts
    
    if (accounts) {
        Display balance
    } else { // History
        Display transaction listings
    }
    
    return
} */