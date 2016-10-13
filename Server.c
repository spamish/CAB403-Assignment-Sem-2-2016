#include "Server.h"

/* sock         - id of server socket
 * conn         - id of client socket
 */
int ln_auth, ln_acc, ln_cli, ln_tran, sock = ERROR, conn = 0;
DB_AUTHENTICATON *db_auth;
DB_ACCOUNT *db_acc;
DB_CLIENT *db_cli;
DB_TRANSACTION *db_tran;

int main(int argc, char *argv[])
{
    /* port         - port address to use
     * server_addr  - endpoint for server
     * client_addr  - endpoint for client
     * sock_size    - data size of socket information
     * buffer       - buffer to place text data in
     */
    int loc, port = DEFAULT_PORT;
    struct sockaddr_in server_addr, client_addr;
    socklen_t sock_size;
    char dummy[BUFFSIZE], buffer[BUFFSIZE], accounts[BUFFSIZE], \
            op[INPUTSIZE], cl[INPUTSIZE];
    FILE *file;
    
    // check if port number is given.
    if (argc == 2)
    {
        port = atoi(argv[1]);
    }
    
    // setup interrupt handler
    signal(SIGINT, interrupt);
    printf("Creating database\n");
    
    // Authentication.txt
    file = fopen("./Authentication.txt", "r");
    ln_auth = get_lines(file) - 1;
    fseek(file, 0, SEEK_SET);
    db_auth = malloc(sizeof(DB_AUTHENTICATON) * ln_auth);
    while (fgetc(file) != '\n') {}
    
    for (int i = 0; i < ln_auth; i++)
    {
        fgets(dummy, BUFFSIZE, file);
        loc = strlen(dummy) - 1;
        
        if (dummy[loc] != '\n')
        {
            sscanf(dummy, "%s %s %s", \
                    db_auth[i].username, \
                    db_auth[i].password, \
                    dummy);
            sprintf(db_auth[i].client, "%s%c", dummy, '\0');
        }
        else
        {
            sscanf(dummy, "%s %s %s", \
                    db_auth[i].username, \
                    db_auth[i].password, \
                    db_auth[i].client);
        }
    }
    
    fclose(file);
    
    // Accounts.txt
    file = fopen("./Accounts.txt", "r");
    ln_acc = get_lines(file) - 1;
    fseek(file, 0, SEEK_SET);
    db_acc = malloc(sizeof(DB_ACCOUNT) * ln_acc);
    while (fgetc(file) != '\n') {}
    
    for (int i = 0; i < ln_acc; i++)
    {
        fgets(dummy, BUFFSIZE, file);
        sscanf(dummy, "%s %s %s ", \
                db_acc[i].number, \
                op, \
                cl);
        
        db_acc[i].opening = atof(op);
        db_acc[i].closing = atof(cl);
    }
    
    fclose(file);
    
    // Client_Details.txt
    file = fopen("./Client_Details.txt", "r");
    ln_cli = get_lines(file) - 1;
    fseek(file, 0, SEEK_SET);
    db_cli = malloc(sizeof(DB_CLIENT) * ln_cli);
    while (fgetc(file) != '\n') {}
    
    for (int i = 0; i < ln_cli; i++)
    {
        fgets(dummy, BUFFSIZE, file);
        sscanf(dummy, "%s %s %s %s ", \
                    db_cli[i].firstname, \
                    db_cli[i].lastname, \
                    db_cli[i].client, \
                    accounts);
        
        loc = strlen(accounts);
        for (int j = 0; j < 3; j++)
        {
            if (loc >= ((j + 1) * 8 + j))
            {
                strncpy(db_cli[i].accounts[j], &accounts[j*8 + j], 8);
                db_cli[i].accounts[j][8] = '\0';
            }
            else
            {
                strncpy(db_cli[i].accounts[j], "0", 9);
                db_cli[i].accounts[j][1] = '\0';
            }
        }
    }
    
    fclose(file);
    
    // Transactions.txt
    file = fopen("./Transactions.txt", "r");
    ln_tran = get_lines(file) - 1;
    
    if (ln_tran > 0)
    {
        fseek(file, 0, SEEK_SET);
        db_tran = malloc(sizeof(DB_TRANSACTION) * ln_tran);
        while (fgetc(file) != '\n') {}
        
        for (int i = 0; i < ln_tran; i++)
        {
            fgets(dummy, BUFFSIZE, file);
            sscanf(dummy, "%s %s %s %s ", \
                    db_tran[i].from, \
                    db_tran[i].to, \
                    op, \
                    cl);
            
            db_tran[i].type = op[0];
            db_tran[i].amount = atof(cl);
        }
    }
    
    fclose(file);
    printf("Starting server\n");
    
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
    sock_size = sizeof(struct sockaddr_in);
    
    // run indefinitely
    while (TRUE)
    {
        printf("Waiting for Client\n");
        
        // accept connection
        if ((conn = accept(sock, (struct sockaddr *) &client_addr, \
                &sock_size)) == ERROR)
        {
            perror("Problem accepting connection");
            continue;
        }
        
        sprintf(buffer, "%d%c", conn, '\0');
        
        // send id to user for future expansion and confirmation
        if (send(conn, buffer, strlen(buffer), 0) == ERROR)
        {
            perror("Problem sending id");
            close(conn);
            continue;
        }
        
        printf("Client connected\n");
        
        // client connected and doing shit
        while(client_actions(conn)) {}
        
        printf("Disconnecting client\n");
        
        // close client connection
        close(conn);
        conn = 0;
    }
}

int get_lines(FILE *data)
{
    int chst, chnd = 0, chrd = 0, num = 0;
    do
    {
        chst = fgetc(data);
        if (chst == EOF)
        {
            break;
        }
        
        if (chst == '\n')
        {
            num++;
        }
        
        chrd = chnd;
        chnd = chst;
        
    } while (chst != EOF);
    
    if (chnd != '\n')
    {
        num += 1;
    }
    
    return num;
}

int client_actions(int id)
{
    int num;
    char rec[BUFFSIZE];
    char sen[BUFFSIZE];
    
    // retreive client instruction
    if ((num = read(id, rec, BUFFSIZE)) > FALSE)
    {
        rec[num] = '\0';
    }
    
    if (num <= FALSE)
    {
        printf("Problem receiving instruction\n");
        return FALSE;
    }
    
    //printf("\tins: %s, siz: %d\n", rec, strlen(rec));
    
    // command goes off for processing
    handler(sen, rec, id);
    
    if (strlen(sen) > 0)
    {
        //printf("\tres: %s, siz: %d\n", sen, strlen(sen));
        
        // send response (might change to within handler)
        if (send(id, sen, strlen(sen), 0) == ERROR)
        {
            perror("Problem sending response");
            return FALSE;
        }
        
        // check if pass or fail received
        sprintf(rec, "%d%c", EXIT, '\0');
        if (!strcmp(sen, rec))
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

void handler(char *sen, char *rec, int id)
{
    size_t siz;
    int lookup = TRUE, arguments = ARGS, prog = 0;
    char *temp, **args = malloc(sizeof(*args)*arguments);
    memset(args, '\0', sizeof(*args)*arguments);
    
    switch (rec[prog++] - PAD)
    {
        case LOGIN:
        // @param username password
        // @return firstname lastname client accounts[3]
        // @fail exit
        {
            arguments = 2;
            lookup = ERROR;
            
            // retrieve username
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[0] = (char *) malloc(siz);
            memcpy(args[0], &rec[prog], siz);
            args[0][siz] = '\0';
            prog += siz;
            
            // retrieve password
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[1] = (char *) malloc(siz);
            memcpy(args[1], &rec[prog], siz);
            args[1][siz] = '\0';
            prog += siz;
            
            // check username and password
            for (int i = 0; i < ln_auth; i++)
            {
                if (!strcmp(args[0], db_auth[i].username))
                {
                    if (!strcmp(args[1], db_auth[i].password))
                    {
                        lookup = i;
                    }
                    
                    break;
                }
            }
            
            // @todo check username and password
            if (lookup == ERROR)
            {
                sprintf(sen, "%c%d%c", EXIT + PAD, FALSE, '\0');
                return;
            }
            
            for (int i = 0; i < ln_auth; i++)
            {
                if (!strcmp(db_auth[lookup].client, db_cli[i].client))
                {
                    lookup = i;
                    break;
                }
            }

            sprintf(sen, "%c%s%c%s%c%s%c%s%c%s%c%s%c", \
                    strlen(db_cli[lookup].firstname) + PAD, \
                    db_cli[lookup].firstname, \
                    strlen(db_cli[lookup].lastname) + PAD, \
                    db_cli[lookup].lastname, \
                    strlen(db_cli[lookup].client) + PAD, \
                    db_cli[lookup].client, \
                    strlen(db_cli[lookup].accounts[0]) + PAD, \
                    db_cli[lookup].accounts[0], \
                    strlen(db_cli[lookup].accounts[1]) + PAD, \
                    db_cli[lookup].accounts[1], \
                    strlen(db_cli[lookup].accounts[2]) + PAD, \
                    db_cli[lookup].accounts[2], '\0');
            
            break;
        }
        
        case ACCOUNTS:
        // @param account
        // @return number opening closing
        // @fail return '0'
        {
            arguments = 3;
            lookup = ERROR;
            
            // retrieve account
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[0] = (char *) malloc(siz);
            memcpy(args[0], &rec[prog], siz);
            args[0][siz] = '\0';
            prog += siz;
            
            // check account
            for (int i = 0; i < ln_acc; i++)
            {
                if (!strcmp(args[0], db_acc[i].number))
                {
                    lookup = i;
                    break;
                }
            }
            
            if (lookup == ERROR)
            {
                sprintf(sen, "%c%d%c", FAIL + PAD, FALSE, '\0');
                return;
            }
            
            // initialise memory for balances
            args[1] = (char *) malloc(INPUTSIZE);
            args[2] = (char *) malloc(INPUTSIZE);
            
            // get strings of balances
            sprintf(args[1], "%.2f%c", db_acc[lookup].opening, '\0');
            sprintf(args[2], "%.2f%c", db_acc[lookup].closing, '\0');
            
            sprintf(sen, "%c%s%c%s%c%s%c%s%c%s%c%s%c", \
                    strlen(db_acc[lookup].number) + PAD, \
                    db_acc[lookup].number, \
                    strlen(args[1]) + PAD, \
                    args[1], \
                    strlen(args[2]) + PAD, \
                    args[2], '\0');
            
            break;
        }
        
        case EXTERNAL:
            lookup = FALSE;
        case TRANSFER:
        // @param from to type amount
        // @return (EXTERNAL closing or TRANSFER from.closing to.closing)
        // @fail return (EXTERNAL or TRANSFER) '0'
        {
            arguments = 4;
            
            // retrieve from
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[0] = (char *) malloc(siz);
            memcpy(args[0], &rec[prog], siz);
            args[0][siz] = '\0';
            prog += siz;
            
            // retrieve to
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[1] = (char *) malloc(siz);
            memcpy(args[1], &rec[prog], siz);
            args[1][siz] = '\0';
            prog += siz;
            
            // retrieve type
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[2] = (char *) malloc(siz);
            memcpy(args[2], &rec[prog], siz);
            args[2][siz] = '\0';
            prog += siz;
            
            // retrieve amount
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[3] = (char *) malloc(siz);
            memcpy(args[3], &rec[prog], siz);
            args[3][siz] = '\0';
            prog += siz;
            
            prog = process_transaction( \
                    args[0], \
                    args[1], \
                    args[2][0], \
                    atof(args[3]), \
                    lookup);
            
            if (prog != TRUE)
            {
                sprintf(sen, "%c%c%c", prog + PAD, FAIL + PAD, '\0');
                
                return;
            }
            
            if ((temp = realloc(args[2], INPUTSIZE)) == NULL)
            {
                printf("Cannot allocate more memory.\n");
                return;
            }
            else
            {
                args[2] = temp;
            }
            
            if ((temp = realloc(args[3], INPUTSIZE)) == NULL)
            {
                printf("Cannot allocate more memory.\n");
                return;
            }
            else
            {
                args[3] = temp;
            }
            
            for (int i = 0; i < ln_acc; i++)
            {
                if (!strcmp(args[0], db_acc[i].number))
                {
                    sprintf(args[2], "%.2f%c", db_acc[i].closing, '\0');
                    break;
                }
            }
            
            for (int i = 0; i < ln_acc; i++)
            {
                if (!strcmp(args[1], db_acc[i].number))
                {
                    sprintf(args[3], "%.2f%c", db_acc[i].closing, '\0');
                    break;
                }
            }
            
            if ((temp = realloc(args[0], sizeof(char))) == NULL)
            {
                printf("Cannot allocate more memory.\n");
                return;
            }
            else
            {
                args[0] = temp;
            }
            
            if (lookup)
            {
                args[0][0] = TRANSFER + PAD;
                sprintf(sen, "%c%c%s%c%s%c", \
                        args[0][0], \
                        strlen(args[2]) + PAD, \
                        args[2], \
                        strlen(args[3]) + PAD, \
                        args[3], '\0');
            }
            else
            {
                args[0][0] = EXTERNAL + PAD;
                sprintf(sen, "%c%c%s%c%s%c", \
                        args[0][0], \
                        strlen(args[2]) + PAD, \
                        args[2], \
                        1 + PAD, \
                        "0", '\0');
            }
            
            break;
        }
        
        case HISTORY:
        // @param account
        // @return array size opening closing
        // @loop fromaccount toaccount type amount
        {
            arguments = 2;
            
            // retrieve account
            siz = (rec[prog++] - PAD) * sizeof(char);
            args[0] = (char *) malloc(siz);
            memcpy(args[0], &rec[prog], siz);
            args[0][siz] = '\0';
            prog = 0;
            
            // find transactions
            if (ln_tran > 0)
            {
                args[1] = (char *) malloc(ln_tran * 2);
                
                for (int i = 0; i < ln_tran; i++)
                {
                    // deposits withdrawals and transactions from
                    if (!strcmp(args[0], db_tran[i].from))
                    {
                        args[1][prog++] = i;
                    }
                    
                    // transactions to
                    if (!strcmp(args[0], db_tran[i].to) && (db_tran[i].type == TRAN))
                    {
                        args[1][prog++] = i;
                    }
                }
            }
            
            send_transactions(id, args[0], prog, args[1]);
            memset(sen, '\0', BUFFSIZE);
            
            break;
        }
        
        case EXIT:
        // @param exit
        // @return exit
        {
            sprintf(sen, "%c%d%c", EXIT + PAD, FALSE, '\0');
            break;
        }
    }
    
    for (int i = 0; i < arguments; i++)
    {
        free(args[i]);
    }
    free(args);
}

int authenticate_client(char *username, char *password)
{
    for (int i = 0; i < ln_auth; i++)
    {
        if (!strcmp(username, db_auth[i].username))
        {
            if (!strcmp(password, db_auth[i].password))
            {
                memset(username, '\0', sizeof(username));
                strncpy(username,  db_auth[i].client, sizeof(db_auth[i].client));
                return FALSE;
            }
            
            return TRUE;
        }
    }
    
    return TRUE;
}

int process_transaction(char *accfrom, char *accto, char type, double amount, int ret)
{
    double buffer = 0.0f;
    DB_ACCOUNT *account;
    DB_TRANSACTION *temp;
    int from, to;
    
    // check above max amount
    if ((type == DEPO) && (amount > MAX_DEPOSIT))
    {
        return TRANSFER;
    }
    
    // assign from account
    for (int i = 0; i < ln_acc; i++)
    {
        if (!strcmp(accfrom, db_acc[i].number))
        {
            account = malloc(sizeof(DB_ACCOUNT));
            memcpy(account[0].number, db_acc[i].number, strlen(accfrom));
            account[0].closing = db_acc[i].closing;
            from = i;
            
            break;
        }
    }
    
    if (type != DEPO)
    {
        // max limit for credit cards
        if (!(atoi(account[0].number) % CRED))
        {
            buffer = MAX_LIMIT;
        }
        
        // check balance
        if (amount > (account[0].closing + buffer))
        {
            free(account);
            return EXTERNAL;
        }
    }
    
    // assign to account
    for (int i = 0; i < ln_acc; i++)
    {
        if (!strcmp(accto, db_acc[i].number))
        {
            to = i;
            break;
        }
    }
    
    // process transaction
    if ((temp = realloc(db_tran, sizeof(DB_TRANSACTION) * (ln_tran + 1))) == NULL)
    {
        printf("Cannot allocate more memory.\n");
        return FALSE;
    }
    else
    {
        db_tran = temp;
        memset(&db_tran[ln_tran], '\0', sizeof(DB_TRANSACTION));
    }
    
    memcpy(db_tran[ln_tran].from, accfrom, sizeof(accfrom));
    memcpy(db_tran[ln_tran].to, accto, sizeof(accto));
    db_tran[ln_tran].type = type;
    
    // update balances
    switch (type)
    {
        case DEPO:
            db_tran[ln_tran].amount = -amount;
            db_acc[from].closing += amount;
            
            break;
        
        case WITH:
            db_tran[ln_tran].amount = amount;
            db_acc[from].closing -= amount;
            
            break;
        
        case TRAN:
            db_tran[ln_tran].amount = -amount;
            db_acc[from].closing -= amount;
            db_acc[to].closing += amount;
            
            break;
    }
    
    ln_tran++;
    free(account);
    return TRUE;
}

void send_transactions(int id, char *account, int count, char *loc)
{
    int num;
    char rec[BUFFSIZE], \
            sen[BUFFSIZE], \
            opening[INPUTSIZE], \
            closing[INPUTSIZE];
    
    // check account
    for (int i = 0; i < ln_acc; i++)
    {
        if (!strcmp(account, db_acc[i].number))
        {
            num = i;
            break;
        }
    }
    
    // get strings of balances
    sprintf(opening, "%.2f%c", db_acc[num].opening, '\0');
    sprintf(closing, "%.2f%c", db_acc[num].closing, '\0');
    
    sprintf(sen, "%c%c%c%s%c%s%c", \
            1 + PAD, \
            count + PAD, \
            strlen(opening) + PAD, \
            opening, \
            strlen(closing) + PAD, \
            closing, '\0');
    
    // send account balance and count
    if (send(id, sen, strlen(sen), 0) == ERROR)
    {
        perror("Problem sending response");
        return;
    }
    
    if (count > 0)
    {
        for (int i = 0; i < count; i++)
        {
            read(id, rec, BUFFSIZE);
            sprintf(rec, "%.2f%c", db_tran[loc[i]].amount, '\0');
            
            sprintf(sen, "%c%s%c%s%c%c%c%s", \
                    strlen(db_tran[loc[i]].from) + PAD, \
                    db_tran[loc[i]].from, \
                    strlen(db_tran[loc[i]].to) + PAD, \
                    db_tran[loc[i]].to, \
                    1 + PAD, \
                    db_tran[loc[i]].type, \
                    strlen(rec) + PAD, \
                    rec, '\0');
            
            send(id, sen, strlen(sen), 0);
        }
    }
}

void interrupt(int dummy)
{
    FILE *file;
    char text[BUFFSIZE];
    
    printf("Shutdown initiated\n");
    
    if (sock != ERROR)
    {
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
    }
    
    printf("Saving data\n");
    
    // Accounts.txt
    file = fopen("./Accounts.txt", "w");
    fseek(file, 0, SEEK_SET);
    fprintf(file, "AccountNo      OpeningBal     ClosingBal\n");
    
    for (int i = 0; i < ln_acc; i++)
    {
        fprintf(file, "%-9s%14.2f%14.2f\n", \
                db_acc[i].number, \
                db_acc[i].opening, \
                db_acc[i].closing);
    }
    
    // @todo sort transactions
    
    // Transactions.txt
    if (ln_tran)
    {
        file = fopen("./Transactions.txt", "w");
        fseek(file, 0, SEEK_SET);
        fprintf(file, "FromAccount      ToAccount       TranType    Amount\n");
        
        for (int i = 0; i < ln_tran; i++)
        {
            fprintf(file, "%-15s%10s%12c%14.2f\n", \
                    db_tran[i].from, \
                    db_tran[i].to, \
                    db_tran[i].type, \
                    db_tran[i].amount);
        }
    }
    
    printf("Exiting program\n");
    
    // free memory
    free(db_auth);
    free(db_acc);
    free(db_cli);
    free(db_tran);
    
    exit(0);
}
