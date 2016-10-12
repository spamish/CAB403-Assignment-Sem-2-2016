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

int with_depo_tran(int sock, int type)
{
    int sel, cont = FALSE, prog = 0, ext = FALSE, dep = FALSE;
    char from[INPUTSIZE], \
            to[INPUTSIZE], \
            input[INPUTSIZE], \
            dummy[INPUTSIZE], \
            message[BUFFSIZE], \
            reply[BUFFSIZE];
    double amount;
    size_t siz;
    DB_ACCOUNT *account = malloc(sizeof(DB_ACCOUNT) * 1);
    
    memset(from, '\0', INPUTSIZE);
    memset(to, '\0', INPUTSIZE);
    memset(input, '\0', INPUTSIZE);
    memset(dummy, '\0', INPUTSIZE);
    
    // select from account
    while (TRUE)
    {
        if (type == DEPO)
        {
            dep = TRUE;
        }
        
        sel = select_account(sock, dep, FALSE, account);
        
        if (sel == FIN)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return FIN;
        }
        
        if (sel != ERROR)
        {
            for (int i = 0; i < 3; i++)
            {
                if (!(atoi(client[0].accounts[i]) % sel))
                {
                    memcpy(from, \
                            client[0].accounts[i], \
                            sizeof(client[0].accounts[i]));
                    break;
                }
            }
            
            break;
        }
        
        // invalid selection
        printf("\n%s\n\n", MISC_FAIL);
    }
    
    // select to account
    if (type == TRAN)
    {
        while (TRUE)
        {
            sel = select_account(sock, TRUE, TRUE, account);
            
            if (sel == FIN)
            {
                // cancel
                printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
                return FIN;
            }
            
            if (sel != ERROR)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (!(atoi(client[0].accounts[i]) % sel))
                    {
                        if (!strcmp(client[0].accounts[i], from))
                        {
                            printf("\n%s\n\n", MISC_FAIL);
                            continue;
                        }
                        
                        memcpy(to, \
                                client[0].accounts[i], \
                                sizeof(client[0].accounts[i]));
                        break;
                    }
                }
                
                break;
            }
            
            // invalid selection
            printf("\n%s\n\n", MISC_FAIL);
        }
    }
    else
    {
        memcpy(to, from, sizeof(from));
    }
    
    // request amount
    memset(dummy, '\0', INPUTSIZE);
    printf("\n%s", AMOUNT_TRANSACTION[type - 2]);
    fgets(input, INPUTSIZE, stdin);
    memcpy(dummy, input, strlen(input) - 1);
    amount = atof(dummy);
    
    // format amount
    memset(dummy, '\0', INPUTSIZE);
    sprintf(dummy, "%.2f", amount);
    printf("from: %s, to: %s, type: %d, amount: %s\n", from, to, type, dummy);
    
    // compile message
    sprintf(message, "%c%c%s%c%s", TRANSFER + PAD, \
            strlen(from) + PAD, \
            from, \
            strlen(to) + PAD, \
            to);
    
    // communicate with server
    talking(sock, message, reply);
    
    // display result
    printf("\nPrint result\n\n%s\n\n", MISC_SEPARATE);
    
    return TRUE;
}

int bala_hist(int sock, int type)
{
    DB_ACCOUNT *account;
    int sel, prog = 0;
    char dummy[INPUTSIZE], message[BUFFSIZE], reply[BUFFSIZE];
    size_t siz;
    
    memset(message, '\0', BUFFSIZE);
    memset(reply, '\0', BUFFSIZE);
    account = malloc(sizeof(DB_ACCOUNT) * 1);
    
    while (TRUE)
    {
        sel = select_account(sock, TRUE, FALSE, account);
        
        if (sel == FIN)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return FIN;
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
            sel = i;
            
            break;
        }
    }
    
    switch (type)
    {
        case BALA:
        // display account balance
        {
            // compile message
            sprintf(message, "%c%c%s%c%s", ACCOUNTS + PAD, \
                    strlen(client[0].accounts[sel]) + PAD, \
                    client[0].accounts[sel]);
            
            // communicate with server
            talking(sock, message, reply);
            
            // assign account information
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(account[0].number, &reply[prog], siz);
            account[0].number[siz] = '\0';
            prog += siz;
            
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(dummy, &reply[prog], siz);
            dummy[siz] = '\0';
            prog += siz;
            
            account[0].opening = atof(dummy);
            
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(dummy, &reply[prog], siz);
            dummy[siz] = '\0';
            prog += siz;
            
            account[0].closing = atof(dummy);
            
            // display account balance
            printf("%s ", BALANCE_CURRENT);
            print_money(account[0].closing);
            printf("\n");
            
            break;
        }
        
        case HIST:
        // @todo display transaction history
        {
            /*// compile message
            sprintf(message, "%c%c%s%c%s", HISTORY + PAD, \
                    strlen(client[0].accounts[sel]) + PAD, \
                    client[0].accounts[sel]);
            
            // communicate with server
            talking(sock, message, reply);
            
            // assign account information
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(account[0].number, &reply[prog], siz);
            account[0].number[siz] = '\0';
            prog += siz;
            
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(dummy, &reply[prog], siz);
            dummy[siz] = '\0';
            prog += siz;
            
            account[0].opening = atof(dummy);
            
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(dummy, &reply[prog], siz);
            dummy[siz] = '\0';
            prog += siz;
            
            account[0].closing = atof(dummy);
            
            // display account balance
            printf("%s ", BALANCE_CURRENT);
            print_money(account[0].closing);
            printf("\n");
            
            // display transaction listings
            printf("%s %d\n\n%s $%.2f\n\n%s $%.2f\n", \
                    HISTORY_TOTAL, \
                    1, \
                    HISTORY_OPEN, \
                    account[0].opening, \
                    HISTORY_CLOSE, \
                    account[0].closing); // @todo get transaction listings
            */
            
            break;
        }
    }
    
    printf("\n%s ", MISC_CONTINUE);
    fgets(dummy, INPUTSIZE, stdin);
    printf("\n%s\n\n", MISC_SEPARATE);
    return TRUE;
}

int select_account(int sock, int stage, int ext, DB_ACCOUNT *account)
{
    char input[INPUTSIZE], \
            dummy[INPUTSIZE], \
            message[BUFFSIZE], \
            reply[BUFFSIZE];
    int prog, sel[4] = {0, 0, 0, 0};
    size_t siz;
    
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
                printf(" %s\n", ACCOUNTS_TYPE[SAVE - SCALE]);
            }
            
            // loan account
            if (!(atoi(client[0].accounts[i - 1]) % LOAN) && stage)
            {
                sel[0]++;
                sel[sel[0]] = LOAN;
                numbering(sel[0]);
                printf(" %s\n", ACCOUNTS_TYPE[LOAN - SCALE]);
            }
            
            // credit card
            if (!(atoi(client[0].accounts[i - 1]) % CRED))
            {
                sel[0]++;
                sel[sel[0]] = CRED;
                numbering(sel[0]);
                printf(" %s\n", ACCOUNTS_TYPE[CRED - SCALE]);
            }
        }
    }
    
    // get results
    printf("\n%s ", MISC_SELECT);
    fgets(input, INPUTSIZE, stdin);
    memcpy(dummy, input, strlen(input) - 1);
    
    // cancel
    if ((CANCEL == dummy[0]) && (strlen(dummy) == 1))
    {
        return FIN;
    }
    
    if (ext && (strlen(dummy) == 8))
    {
        prog = 0;
        
        // compile message
        sprintf(message, "%c%c%s", ACCOUNTS + PAD, \
                strlen(dummy) + PAD, \
                dummy);
        
        // communicate with server
        talking(sock, message, reply);
        
        if ((reply[0] == FAIL) && (reply[1] == FALSE))
        {
            return ERROR;
        }
        
        // assign account information
        siz = (reply[prog++] - PAD) * sizeof(char);
        memcpy(account[0].number, &reply[prog], siz);
        account[0].number[siz] = '\0';
        prog += siz;
        
        siz = (reply[prog++] - PAD) * sizeof(char);
        memcpy(dummy, &reply[prog], siz);
        dummy[siz] = '\0';
        prog += siz;
        
        account[0].opening = atof(dummy);
        
        siz = (reply[prog++] - PAD) * sizeof(char);
        memcpy(dummy, &reply[prog], siz);
        dummy[siz] = '\0';
        prog += siz;
        
        account[0].closing = atof(dummy);
        
        return TRUE;
    }
    
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

void print_money(double val)
{
    if (val < 0)
    {
        printf("-");
        val = fabs(val);
    }
    
    printf("$%.2f", val);
}