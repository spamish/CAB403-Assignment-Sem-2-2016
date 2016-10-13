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
    client = malloc(sizeof(DB_CLIENT));
    
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
    if ((numbytes = read(conn, reply, BUFFSIZE)) > 0)
    {
        reply[BUFFSIZE] = '\0';
    }
    
    if (numbytes <= 0)
    {
        printf("Problem receiving id\n");
        exit(1);
    }
    
    // communication confirmed
    id = atoi(reply);
    printf("Connected to server\n\n%s\n\n", MISC_SEPARATE);
    
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
    memset(username, '\0', BUFFSIZE);
    memset(password, '\0', BUFFSIZE);
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
    sprintf(message, "%c%c%s%c%s%c", LOGIN + PAD, \
            strlen(username) + PAD, username, \
            strlen(password) + PAD, password, '\0');
    
    // communicate with server
    if(talking(sock, message, reply) == FIN)
    {
        printf("Problem logging in\n");
        exit(1);
    }
    
    // check if pass or fail received
    if ((reply[0] == (EXIT + PAD)) && (reply[1] == (FALSE + PAD)))
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
        printf("Problem sending message, logging out\n\n");
        return FIN;
    }
    
    //printf("\tsen: %s\n", sen);
    
    // read response
    if ((num = read(sock, rec, BUFFSIZE)) > 0)
    {
        rec[BUFFSIZE] = '\0';
    }
    
    // check if response is bullshit
    if (num <= 0)
    {
        printf("Problem receiving response, logging out\n\n");
        return FIN;
    }
    
    //printf("\trec: %s\n", rec);
    
    return TRUE;
}

int auto_mach_tell(int sock)
{
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
    switch((char) (atoi(input) + PAD))
    {
        case BALA: // balance
            if (balance(sock) == FIN)
            {
                return FALSE;
            }
            
            break;
        
        case DEPO: // deposit
            if (with_depo_tran(sock, DEPO) == FIN)
            {
                return FALSE;
            }
            
            break;
        
        case WITH: // withdrawal
            if (with_depo_tran(sock, WITH) == FIN)
            {
                return FALSE;
            }
            
            break;
        
        case TRAN: // transfer
            if (with_depo_tran(sock, TRAN) == FIN)
            {
                return FALSE;
            }
            
            break;
        
        case HIST: // history
            if (history(sock) == FIN)
            {
                return FALSE;
            }
            
            break;
        
        case (EXIT + PAD):
            //return exit
            sprintf(message, "%c%d%c", EXIT + PAD, FALSE, '\0');
            
            // communicate with server
            talking(sock, message, reply);
            return FALSE;
        
        default:
            printf("%s\n\n", MISC_FAIL);
    }
    
    return TRUE;
}

int with_depo_tran(int sock, char type)
{
    int sel, cont, prog = 0, ext = FALSE, dep = FALSE;
    char resp, \
            from[INPUTSIZE], \
            to[INPUTSIZE], \
            input[INPUTSIZE], \
            dummy[INPUTSIZE], \
            message[BUFFSIZE], \
            reply[BUFFSIZE];
    double amount;
    size_t siz;
    DB_ACCOUNT *account = malloc(sizeof(DB_ACCOUNT));
    
    memset(from, '\0', INPUTSIZE);
    memset(to, '\0', INPUTSIZE);
    memset(input, '\0', INPUTSIZE);
    memset(dummy, '\0', INPUTSIZE);
    
    // select from account
    cont = TRUE;
    while (cont)
    {
        if (type == DEPO)
        {
            dep = TRUE;
        }
        
        sel = select_account(sock, dep, FALSE, account);
        
        if (sel == FIN)
        {
            return FIN;
        }
        
        if (sel == FALSE)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return FALSE;
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
                    cont = FALSE;
                }
            }
        }
        
        // invalid selection
        if (cont)
        {
            printf("\n%s\n\n", MISC_FAIL);
        }
    }
    
    printf("\n");
    
    // select to account
    if (type == TRAN)
    {
        cont = TRUE;
        while (cont)
        {
            sel = select_account(sock, TRUE, TRUE, account);
            
            if (sel == FIN)
            {
                return FIN;
            }
            
            if (sel == FALSE)
            {
                // cancel
                printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
                return FALSE;
            }
            
            if (sel == TRUE)
            {
                memcpy(to, account, sizeof(account));
                cont = FALSE;
            }
            else
            {
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
                            sel = FALSE;
                            cont = FALSE;
                            break;
                        }
                    }
                }
            }
            
            if (cont)
            {
                // invalid selection
                printf("\n%s\n\n", MISC_FAIL);
            }
        }
        
        printf("\n");
    }
    else
    {
        memcpy(to, from, sizeof(from));
    }
    
    cont = TRUE;
    while(cont)
    {
        // request amount
        switch (type)
        {
            case DEPO:
                printf("%s", AMOUNT_DEPOSIT);
                break;
            case WITH:
                printf("%s", AMOUNT_WITHDRAWAL);
                break;
            case TRAN:
                printf("%s", AMOUNT_TRANSFER);
                break;
        }
        
        fgets(input, INPUTSIZE, stdin);
        memset(dummy, '\0', INPUTSIZE);
        memcpy(dummy, input, strlen(input) - 1);
        
        // cancel
        if ((CANCEL == dummy[0]) && (strlen(dummy) == 1))
        {
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return FALSE;
        }
        
        // format amount
        amount = atof(dummy);
        
        if (amount > 0.0)
        {
            sprintf(dummy, "%.2f%c", amount, '\0');
            
            // compile message
            if (sel)
            {
                sprintf(message, "%c%c%s%c%s%c%c%c%s%c", EXTERNAL + PAD, \
                        strlen(from) + PAD, \
                        from, \
                        strlen(to) + PAD, \
                        to, \
                        sizeof(type) + PAD, \
                        type, \
                        strlen(dummy) + PAD, \
                        dummy, '\0');
            }
            else
            {
                sprintf(message, "%c%c%s%c%s%c%c%c%s%c", TRANSFER + PAD, \
                        strlen(from) + PAD, \
                        from, \
                        strlen(to) + PAD, \
                        to, \
                        sizeof(type) + PAD, \
                        type, \
                        strlen(dummy) + PAD, \
                        dummy, '\0');
            }
            
            // communicate with server
            if (talking(sock, message, reply) == FIN)
            {
                return FIN;
            }
            
            prog = 0;
            resp = reply[prog++] - PAD;
            
            if ((reply[0] == (FAIL + PAD)) && (reply[1] == (FALSE + PAD)))
            {
                switch (resp)
                {
                    case TRANSFER:
                        printf("\n%s\n", ERROR_LOADED);
                        break;
                    
                    case EXTERNAL:
                        printf("\n%s\n", ERROR_BROKE);
                        break;
                }
            }
            else
            {
                cont = FALSE;
            }
        }
        else
        {
            // invalid selection
            printf("\n%s\n\n", ERROR_AMOUNT);
        }
    }
    
    printf("\n%s\n", MISC_SEPARATE);
    
    // display result
    switch (type)
    {
        case DEPO:
            printf("\n%s ", RESULT_DEPOSIT);
            print_money(amount, FALSE);
            
            break;
        case WITH:
            printf("\n%s ", RESULT_WITHDRAWAL);
            print_money(amount, FALSE);
            
            break;
        case TRAN:
            printf("\n%s ", RESULT_TRANSFER);
            print_money(amount, FALSE);
            
            break;
    }
    
    // assign from balance
    memset(dummy, '\0', INPUTSIZE);
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(dummy, &reply[prog], siz);
    dummy[siz] = '\0';
    prog += siz;
    
    // display from balance
    amount = atof(dummy);
    printf("\n%s %s is: ", RESULT_BALANCE, from);
    print_money(amount, FALSE);
    
    // assign to balance
    memset(dummy, '\0', INPUTSIZE);
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(dummy, &reply[prog], siz);
    dummy[siz] = '\0';
    prog += siz;
    
    // display to balance
    if (type == TRAN)
    {
        printf("\n%s %s", RESULT_RECEIVING, to);
        
        if (dummy[0] != '0')
        {
            amount = atof(dummy);
            printf(" is: ");
            print_money(amount, FALSE);
        }
    }
    
    printf("\n\n%s ", MISC_CONTINUE);
    fgets(dummy, INPUTSIZE, stdin);
    printf("\n%s\n\n", MISC_SEPARATE);
    
    return TRUE;
}

int balance(int sock)
{
    DB_ACCOUNT *account;
    int sel, num, prog = 0;
    char dummy[INPUTSIZE], message[BUFFSIZE], reply[BUFFSIZE];
    size_t siz;
    
    memset(message, '\0', BUFFSIZE);
    memset(reply, '\0', BUFFSIZE);
    account = malloc(sizeof(DB_ACCOUNT));
    
    // select account
    while (TRUE)
    {
        sel = select_account(sock, TRUE, FALSE, account);
        
        if (sel == FIN)
        {
            return FIN;
        }
        
        if (sel == FALSE)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return FALSE;
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
    
    // compile message
    sprintf(message, "%c%c%s%c%s%c", ACCOUNTS + PAD, \
            strlen(client[0].accounts[sel]) + PAD, \
            client[0].accounts[sel], '\0');
    
    // communicate with server
    if (talking(sock, message, reply) == FIN)
    {
        return FIN;
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
    
    // display account balance
    printf("%s ", BALANCE_CURRENT);
    print_money(account[0].closing, FALSE);
    printf("\n");
    
    printf("\n%s ", MISC_CONTINUE);
    fgets(dummy, INPUTSIZE, stdin);
    printf("\n%s\n\n", MISC_SEPARATE);
    return TRUE;
}

int history(int sock)
{
    DB_ACCOUNT *account;
    char type, from[9], to[9];
    double amount;
    int sel, num, prog = 0;
    char dummy[INPUTSIZE], message[BUFFSIZE], reply[BUFFSIZE];
    size_t siz;
    
    memset(message, '\0', BUFFSIZE);
    memset(reply, '\0', BUFFSIZE);
    account = malloc(sizeof(DB_ACCOUNT));
    
    // select account
    while (TRUE)
    {
        sel = select_account(sock, TRUE, FALSE, account);
        
        if (sel == FIN)
        {
            return FIN;
        }
        
        if (sel == FALSE)
        {
            // cancel
            printf("\n%s\n\n%s\n\n", MISC_RETURN, MISC_SEPARATE);
            return FALSE;
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
    
    // compile message
    sprintf(message, "%c%c%s%c", HISTORY + PAD, \
            strlen(client[0].accounts[sel]) + PAD, \
            client[0].accounts[sel], '\0');
    
    // communicate with server
    if (talking(sock, message, reply) == FIN)
    {
        return FIN;
    }
    
    // assign account information
    siz = (reply[prog++] - PAD) * sizeof(char);
    memcpy(dummy, &reply[prog], siz);
    dummy[siz] = '\0';
    prog += siz;
    
    num = dummy[0] - PAD;
    
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
    
    // display balances listings
    printf("%s %d\n%s ", HISTORY_TOTAL, num, HISTORY_OPEN);
    print_money(account[0].opening, FALSE);
    printf("\n%s ", HISTORY_CLOSE);
    print_money(account[0].closing, FALSE);
    printf("\n");
    
    if (num > 0)
    {
        // display transactions
        printf("\n%s\n", HISTORY_TABLE);
        
        for (int i = 0; i < num; i++)
        {
            memset(reply, '\0', BUFFSIZE);
            sprintf(message, "%c%c%c", i + PAD, num + PAD, '\0');
            prog = 0;
            
            // request next transaction
            if (talking(sock, message, reply) == FIN)
            {
                return FIN;
            }
            
            // retrieve from
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(from, &reply[prog], siz);
            from[siz] = '\0';
            prog += siz;
            
            // retrieve to
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(to, &reply[prog], siz);
            to[siz] = '\0';
            prog += siz;
            
            // retrieve type
            siz = (reply[prog++] - PAD) * sizeof(char);
            type = reply[prog++];
            
            // retrieve amount
            siz = (reply[prog++] - PAD) * sizeof(char);
            memcpy(dummy, &reply[prog], siz);
            dummy[siz] = '\0';
            prog += siz;
            
            amount = atof(dummy);
            
            switch (type)
            {
                case DEPO:
                    printf("%6d%20s", i + 1, HISTORY_DEPOSIT);
                    print_money(-amount, TRUE);
                    break;
                
                case WITH:
                    printf("%6d%20s", i + 1, HISTORY_WITHDRAW);
                    print_money(-amount, TRUE);
                    break;
                
                case TRAN:
                    printf("%6d%20s", i + 1, HISTORY_TRANSFER);
                    
                    if (strcmp(client[0].accounts[sel], from))
                    {
                        print_money(-amount, TRUE);
                    }
                    else
                    {
                        print_money(amount, TRUE);
                    }
                    
                    break;
            }
            
            printf("\n");
        }
    }
    else
    {
        printf("\n%s\n", HISTORY_NONE);
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
    
    if (ext)
    {
        printf("%s\n\n", ACCOUNTS_TRANSFER);
    }
    else
    {
        printf("%s\n\n", ACCOUNTS_LIST);
    }
    
    // display accounts
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
    
    // get input
    printf("\n%s ", MISC_SELECT);
    fgets(input, INPUTSIZE, stdin);
    memcpy(dummy, input, strlen(input) - 1);
    
    // cancel
    if ((CANCEL == dummy[0]) && (strlen(dummy) == 1))
    {
        return FALSE;
    }
    
    // if including external accounts
    if (ext && (strlen(dummy) == 8))
    {
        prog = 0;
        
        for (int i = 0; i < 3; i++)
        {
            if (!strcmp(dummy, client[0].accounts[i]))
            {
                return ERROR;
            }
        }
        
        // compile message
        sprintf(message, "%c%c%s%c", ACCOUNTS + PAD, \
                strlen(dummy) + PAD, \
                dummy, '\0');
        
        // communicate with server
        if (talking(sock, message, reply) == FIN)
        {
            return FIN;
        }
        
        // invalid account number
        if ((reply[0] == (FAIL + PAD)) && (reply[1] == (FALSE + PAD)))
        {
            return ERROR;
        }
        
        // assign account information
        siz = (reply[prog++] - PAD) * sizeof(char);
        memcpy(account[0].number, &reply[prog], siz);
        account[0].number[siz] = '\0';
        prog += siz;
        
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

void print_money(double val, int pad)
{
    char string[INPUTSIZE], sign = ' ';
    
    if (val < 0)
    {
        sign = '-';
        val = fabs(val);
    }
    
    if (pad)
    {
        sprintf(string, "%c$%.2f%c", sign, val, '\0');
        printf("%20s", string);
    }
    else
    {
        printf("%c$%.2f", sign, val);
    }
}