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
            sprintf(db_auth[i].client, "%s%c", dummy, '\n');
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
    printf("number of lines in Authentication.txt = %d\n", ln_auth);
    
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
    printf("number of lines in Accounts.txt = %d\n", ln_acc);
    
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
    printf("number of lines in Client_Details.txt = %d\n", ln_cli);
    
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
            sscanf(dummy, "%s %s %d %s ", \
                    db_tran[i].from, \
                    db_tran[i].to, \
                    db_tran[i].type, \
                    op);
            
            db_tran[i].amount = atof(op);
        }
    }
    
    fclose(file);
    printf("number of lines in Transactions.txt = %d\n", ln_tran);
    
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
        
        printf("Disconnecting client, id: %d\n", conn);
        
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
    // BUFFERS!
    memset(rec, '\0', BUFFSIZE);
    memset(sen, '\0', BUFFSIZE);
    
    // retreive client instruction
    if ((num = read(id, rec, BUFFSIZE)) > FIN)
    {
        rec[num] = '\0';
    }
    
    if (num <= FIN)
    {
        printf("Problem receiving instruction\n");
        return FALSE;
    }
    
    printf("ins: %s, siz: %d\n", rec, strlen(rec));
    
    // command goes off for processing
    handler(sen, rec);
    
    printf("rep: %s, siz: %d\n", sen, strlen(sen));
    
    // send response (might change to within handler)
    if (send(id, sen, strlen(sen), 0) == ERROR)
    {
        perror("Problem sending response");
        return FALSE;
    }
    
    // check if pass or fail received
    sprintf(rec, "%d", EXIT);
    if (!strcmp(sen, rec))
    {
        return FALSE;
    }
    
    // nothing fucked up, wait for more from client
    return TRUE;
}

void handler(char *sen, char *rec)
{
    size_t siz;
    int lookup, args = ARGS, prog = 0;
    char **arguments = malloc(sizeof(*arguments)*args);
    memset(arguments, '\0', sizeof(*arguments)*args);
    memset(sen, '\0', BUFFSIZE);
    
    switch (rec[prog++] - PAD)
    {
        case LOGIN:
        // @param username password
        // @return firstname lastname client accounts[3]
        // @fail exit
        {
            args = 2;
            lookup = ERROR;
            
            // retrieve username
            siz = (rec[prog++] - PAD) * sizeof(char);
            arguments[0] = (char *) malloc(siz);
            memcpy(arguments[0], &rec[prog], siz);
            arguments[0][siz] = '\0';
            prog += siz;
            
            // retrieve password
            siz = (rec[prog++] - PAD) * sizeof(char);
            arguments[1] = (char *) malloc(siz);
            memcpy(arguments[1], &rec[prog], siz);
            arguments[1][siz] = '\0';
            prog += siz;
            
            // check username and password
            for (int i = 0; i < ln_auth; i++)
            {
                if (!strcmp(arguments[0], db_auth[i].username))
                {
                    if (!strcmp(arguments[1], db_auth[i].password))
                    {
                        lookup = i;
                    }
                    
                    break;
                }
            }
            
            // @todo check username and password
            if (lookup == ERROR)
            {
                sprintf(sen, "%c", EXIT + PAD);
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

            sprintf(sen, "%c%s%c%s%c%s%c%s%c%s%c%s", \
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
                    db_cli[lookup].accounts[2]);
            
            break;
        }
        
        case ACCOUNTS:
        // @param account
        // @return number opening closing
        // @fail return '0'
        {
            args = 3;
            lookup = ERROR;
            
            // retrieve account
            siz = (rec[prog++] - PAD) * sizeof(char);
            arguments[0] = (char *) malloc(siz);
            memcpy(arguments[0], &rec[prog], siz);
            arguments[0][siz] = '\0';
            prog += siz;
            
            // check account
            for (int i = 0; i < ln_acc; i++)
            {
                if (!strcmp(arguments[0], db_acc[i].number))
                {
                    lookup = i;
                    break;
                }
            }
            
            if (lookup == ERROR)
            {
                sprintf(sen, "%c%d", FAIL + PAD, FALSE);
                return;
            }
            
            // initialise memory for balances
            arguments[1] = (char *) malloc(INPUTSIZE);
            arguments[2] = (char *) malloc(INPUTSIZE);
            
            // get strings of balances
            sprintf(arguments[1], "%.2f%c", db_acc[lookup].opening, '\0');
            sprintf(arguments[2], "%.2f%c", db_acc[lookup].closing, '\0');
            
            sprintf(sen, "%c%s%c%s%c%s%c%s%c%s%c%s", \
                    strlen(db_acc[lookup].number) + PAD, \
                    db_acc[lookup].number, \
                    strlen(arguments[1]) + PAD, \
                    arguments[1], \
                    strlen(arguments[2]) + PAD, \
                    arguments[2]);
            
            break;
        }
        
        case TRANSFER:
        // @param 
        // @return 
        {
            
            // @todo check account number
            sprintf(sen, "%c", TRANSFER + PAD);
            break;
        }
        
        case HISTORY:
        // @param accountno
        // @return array size array[fromaccount toaccount type amount]
        {
            
            sprintf(sen, "%c", HISTORY + PAD);
            break;
        }
        
        case FINISH:
        // @param 
        // @return finish
        {
            
            // @todo move transaction from temporary to permanent data
            sprintf(sen, "%c", FINISH + PAD);
            break;
        }
        
        case EXIT:
        // @param exit
        // @return exit
        {
            
            sprintf(sen, "%c\0", EXIT + PAD);
            break;
        }
    }
    
    for (int i = 0; i < args; i++)
    {
        free(arguments[i]);
    }
    free(arguments);
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
                    db_tran[i].type[0], \
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
