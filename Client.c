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
    int conn, port, numbytes;
    struct sockaddr_in conn_addr;
    struct hostent *ip_address;
    char buffer[BUFFSIZE];
    
    // check input parameters
    if (argc != 3) {
        perror("Not enough input arguments");
        exit(1);
    }
    
    // get the ip address
    //if ((ip_address = getaddrinfo(argv[1])) == NULL) {
    //    perror("")
    //}
    
    port = atoi(argv[2]);
    
    // Make socket and connect
    
    // Success {
        
        // exit_status = auto_mach_tell(params)
        
    // }
    
    // Shutdown and close connection
}

/* return auto_mach_tell(params) {
    
    Display welcome message
    
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
    }
} */

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