#include "Client.h"
#include "Textable.h"

DB_CLIENT *client;

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
    char message[BUFFSIZE], reply[BUFFSIZE];
    client = malloc(sizeof(DB_CLIENT) * 1);
    
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
    if (connect(conn, (struct sockaddr *) &conn_addr, \
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
    printf("Connected to server with id: %d\n\n%s\n\n", id, MISC_SEPARATE);
    
    // welcome screen and login
    if (welcome_login(conn))
    {
        printf("\n%s\n", LOGIN_FAIL);
        exit(1);
    }
    
    printf("\n%s\n\n%s %s %s, %s %s\n\n", \
            MISC_SEPARATE, \
            LANDING_NAME, \
            client[0].firstname, \
            client[0].lastname, \
            LANDING_ID, \
            client[0].client);
    
    // logged in and doing shit
    while (auto_mach_tell(conn)) {}
    
    printf("%s\n", LANDING_EXIT);
    free(client);
}

int welcome_login(int sock)
{
    // MOAR BUFFERS!
    char message[BUFFSIZE], reply[BUFFSIZE], \
            username[INPUTSIZE], password[INPUTSIZE], \
            input[INPUTSIZE];
    size_t siz;
    int prog = 0;
    
    memset(message, '\0', BUFFSIZE);
    memset(reply, '\0', BUFFSIZE);
    memset(input, '\0', INPUTSIZE);
    
    // enter username
    printf("%s ", LOGIN_WELCOME);
    fgets(input, INPUTSIZE, stdin);
    memcpy(username, input, strlen(input) - 1);
    
    memset(input, '\0', INPUTSIZE);
    // enter password
    printf("%s ", LOGIN_PASSWORD);
    fgets(input, INPUTSIZE, stdin);
    memcpy(password, input, strlen(input) - 1);
    
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
    sprintf(message, "%d", EXIT);
    if (!strcmp(reply, message))
    {
        return TRUE;
    }
    
    // assign client information
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(client[0].firstname, &reply[prog], siz);
    client[0].firstname[siz] = '\0';
    prog += siz;
    
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(client[0].lastname, &reply[prog], siz);
    client[0].lastname[siz] = '\0';
    prog += siz;
    
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(client[0].client, &reply[prog], siz);
    client[0].client[siz] = '\0';
    prog += siz;
    
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(client[0].accounts[0], &reply[prog], siz);
    client[0].accounts[0][siz] = '\0';
    prog += siz;
    
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(client[0].accounts[1], &reply[prog], siz);
    client[0].accounts[1][siz] = '\0';
    prog += siz;
    
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(client[0].accounts[2], &reply[prog], siz);
    client[0].accounts[2][siz] = '\0';
    prog += siz;
    
    return FALSE;
}

int talking(int sock, char *sen, char *rec)
{
    int num;
    memset(rec, '\0', sizeof(rec));
    
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
    int sel;
    char input[INPUTSIZE], selection[INPUTSIZE], \
            message[BUFFSIZE], reply[BUFFSIZE];
    
    memset(input, '\0', INPUTSIZE);
    memset(selection, '\0', INPUTSIZE);
    memset(message, '\0', BUFFSIZE);
    memset(reply, '\0', BUFFSIZE);
    
    // display menu and request input
    printf("%s ", LANDING_MENU);
    fgets(input, INPUTSIZE, stdin);
    memcpy(selection, input, strlen(input) - 1);
    printf("\n%s\n\n", MISC_SEPARATE);
    
    // @todo select option from main menu
    switch(atoi(input))
    {
        case BALA: // balance
            sel = bala_hist(sock, BALA);
            break;
        
        case DEPO: // deposit
            with_depo_tran(sock, DEPO);
            break;
        
        case WITH: // withdrawal
            with_depo_tran(sock, WITH);
            break;
        
        case TRAN: // transfer
            with_depo_tran(sock, TRAN);
            break;
        
        case HIST: // history
            bala_hist(sock, HIST);
            break;
        
        case EXIT:
            //return exit
            sprintf(message, "%c", EXIT + PAD);
            
            // communicate with server
            talking(sock, message, reply);
            return FALSE;
        
        default:
            printf("%s\n\n", MISC_FAIL);
    }
    
    return TRUE;
}

// @todo withdrawals, deposits and transactions
int with_depo_tran(int sock, int type)
{
    int from, to, ext = FALSE, dep = FALSE;
    char input[INPUTSIZE], dummy[INPUTSIZE];
    double amount;
    
    memset(input, '\0', INPUTSIZE);
    memset(dummy, '\0', INPUTSIZE);
    
    // select from account
    while (TRUE)
    {
        if (type == DEPO)
        {
            dep = TRUE;
        }
        
        from = select_account(dep, FALSE, dummy);
        
        if (from == FIN)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return from;
        }
        
        if (from != ERROR)
        {
            break;
        }
        
        // invalid selection
        printf("\n%s\n\n", MISC_FAIL);
    }
    
    // select to account
    if (type == 4)
    {
        while (TRUE)
        {
            to = select_account(TRUE, TRUE, dummy);
            
            if (to == FIN)
            {
                // cancel
                printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
                return to;
            }
            
            if (to == from)
            {
                printf("\n%s\n\n", MISC_FAIL);
                continue;
            }
            
            if (to != ERROR)
            {
                break;
            }
            
            // invalid selection
            printf("\n%s\n\n", MISC_FAIL);
        }
    }
    
    /*
    if (strlen(dummy))
    {
        ext = TRUE;
    }
    */
    
    // get from account details
    
    // request amount
    while (TRUE)
    {
        printf("\n%s ", AMOUNT_TRANSACTION[type - 2]);
        fgets(input, INPUTSIZE, stdin);
        memcpy(dummy, input, strlen(input) - 1);
        amount = atof(dummy);
        
        /*
        if (too much)
        {
            printf LOADED
            continue;
        }
        */
        
        /*
        if (too little)
        {
            printf BROKE
            continue;
        }
        */
        break;
    }
    
    // display result
    printf("\nPrint result\n\n%s\n\n", MISC_SEPARATE);
    
    return TRUE;
}

// @todo 
int bala_hist(int sock, int type)
{
    int sel;
    char dummy[INPUTSIZE];
    
    while (TRUE)
    {
        sel = select_account(TRUE, FALSE, dummy);
        
        if (sel == FIN)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return sel;
        }
        
        if (sel != ERROR)
        {
            break;
        }
        
        // invalid selection
        printf("\n%s\n\n", MISC_FAIL);
    }
    // display balance
    printf("\n%s\n\n%s %s %s, %s %s\n%s", \
            MISC_SEPARATE, BALANCE_OWNER, \
            client[0].firstname, \
            client[0].lastname, \
            BALANCE_CLIENT, \
            client[0].client, \
            BALANCE_TYPE);
    
    for (int i = 0; i < 3; i++)
    {
        if (!(atoi(client[0].accounts[i]) % sel))
        {
            printf(" %s, %s %s\n\n", \
            ACCOUNTS_TYPE[i], \
            BALANCE_ACCOUNT, \
            client[0].accounts[i]);
        }
    }
    
    switch (type)
    {
        case 1:
            // display account balance
            printf("%s $%.2f\n", \
                    BALANCE_CURRENT, \
                    100.25f); // @todo get account balance
            
            break;
        
        case 5:
            // display transaction listings
            printf("%s %d\n\n%s $%.2f\n\n%s $%.2f\n", \
                    HISTORY_TOTAL, \
                    1, \
                    HISTORY_OPEN, \
                    100.25f, \
                    HISTORY_CLOSE, \
                    100.25f); // @todo get transaction listings
            
            break;
    }
    
    printf("\n%s ", MISC_CONTINUE);
    fgets(dummy, INPUTSIZE, stdin);
    printf("\n%s\n\n", MISC_SEPARATE);
    return TRUE;
}

int select_account(int stage, int ext, char *acc)
{
    char input[INPUTSIZE], dummy[INPUTSIZE];
    int sel[4] = {0, 0, 0, 0};
    
    memset(input, '\0', INPUTSIZE);
    memset(dummy, '\0', INPUTSIZE);
    printf("%s\n\n", ACCOUNTS_LIST);
    
    // loop through accounts
    for (int i = 1; i <= 3; i++)
    {
        if (client[0].accounts[i - 1][0] != '0')
        {
            // savings account
            if (!(atoi(client[0].accounts[i - 1]) % SAVE))
            {
                sel[0]++;
                sel[sel[0]] = SAVE;
                numbering(sel[0]);
                printf(" %s\n", ACCOUNTS_TYPE[0]);
            }
            
            // loan account
            if (!(atoi(client[0].accounts[i - 1]) % LOAN) && stage)
            {
                sel[0]++;
                sel[sel[0]] = LOAN;
                numbering(sel[0]);
                printf(" %s\n", ACCOUNTS_TYPE[1]);
            }
            
            // credit card
            if (!(atoi(client[0].accounts[i - 1]) % CRED))
            {
                sel[0]++;
                sel[sel[0]] = CRED;
                numbering(sel[0]);
                printf(" %s\n", ACCOUNTS_TYPE[2]);
            }
        }
    }
    
    printf("%d %d %d %d\n", sel[0], sel[1], sel[2], sel[3]);
    
    // get results
    printf("\n%s ", MISC_SELECT);
    fgets(input, INPUTSIZE, stdin);
    memcpy(dummy, input, strlen(input) - 1);
    
    // cancel
    if ((CANCEL == dummy[0]) && (strlen(dummy) == 1))
    {
        return FIN;
    }
    
    /*
    if (ext)
    {
        @todo search for matching account number
        if (successful)
        {
            acc = account number
            return TRUE;
        }
    }
    */
    
    // out of range
    if ((atoi(dummy) > sel[0]) || (atoi(dummy) < 1))
    {
        return ERROR;
    }
    
    // result
    return sel[atoi(dummy)];
}

void numbering(int val)
{
    switch (val)
    {
        case 1:
            printf("%s", MISC_ONE);
            break;
        
        case 2:
            printf("%s", MISC_TWO);
            break;
        
        case 3:
            printf("%s", MISC_THREE);
            break;
    }
}
