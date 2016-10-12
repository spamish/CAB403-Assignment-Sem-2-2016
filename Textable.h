// Misc text
const char *MISC_SEPARATE = "================================================================================";

const char *MISC_ONE      = "<1>";

const char *MISC_TWO      = "<2>";

const char *MISC_THREE    = "<3>";

const char *MISC_SELECT   = "Enter Selection or 'c' to cancel:";

const char *MISC_FAIL     = "Invalid selection, please try again";

const char *MISC_CONTINUE = "Press 'enter' to continue";

const char *MISC_RETURN   = "Action cancelled, returning to main menu";

// Login page
const char *LOGIN_WELCOME  = "\
+----------------------------------------------------------+\n\
|                                                          |\n\
|    Welcome to Samuel Janetzki's ATM System for CAB403    |\n\
|                                                          |\n\
+----------------------------------------------------------+\n\
\nTo get started, login with your username and password\n\
\nPlease enter your username:";

const char *LOGIN_PASSWORD = "Please enter your password:";

const char *LOGIN_FAIL     = "You have entered either an incorrect username or password. Please try again later";

// Main menu
const char *LANDING_NAME = "Successfully logged in as:";

const char *LANDING_ID   = "client number:";

const char *LANDING_MENU = "\
Please select from the following options\n\n\
<1> Account Balance\n\
<2> Deposit\n\
<3> Withdrawal\n\
<4> Transfer\n\
<5> Transaction History\n\
<6> Exit System\n\n\
Enter your selection:";

const char *LANDING_EXIT = "Thank you for choosing this ATM, enjoy your day.";

// Account selection
const char *ACCOUNTS_LIST     = "Select account type";

const char *ACCOUNTS_TYPE[3]  = {\
"Savings Account",\
"Loan Account",\
"Credit Card Account"};

const char *ACCOUNTS_TRANSFER = "Select destination account";

// Balance display
const char *BALANCE_OWNER    = "Account owned by:";

const char *BALANCE_CLIENT   = "client number:";

const char *BALANCE_ACCOUNT  = "account number:";

const char *BALANCE_TYPE     = "Account type:";

const char *BALANCE_CURRENT  = "Current balance:";

// Transaction history - grabs several texts from Balance
const char *HISTORY_TOTAL    = "Number of transactions:";

const char *HISTORY_OPEN     = "Opening balance:";

const char *HISTORY_CLOSE    = "Closing balance:";

const char *HISTORY_TABLE    = "Transaction   Type   Amount";

const char *HISTORY_WITHDRAW = "Withdrawal";

const char *HISTORY_DEPOSIT  = "Deposit";

const char *HISTORY_TRANSFER = "Transfer";

const char *HISTORY_NONE     = "No transactions have occured yet";

// Amount entering
const char *AMOUNT_TRANSACTION[3] = {\
"Enter the amount to deposit or 'c' to cancel: $",\
"Enter the amount to withdraw or 'c' to cancel: $",\
"Enter the amount to transfer or 'c' to cancel: $"};

// Result screens
const char *RESULT_TRANSACTION[3]  = {\
"Deposit successful, account balance is:",\
"Withdrawal successful, account balance is:",\
"Transfer successful\nFrom account balance is:"};

const char *RESULT_RECEIVING = "To account balance is:";

const char *RESULT_EXTERNAL  = "Transfer successful, account balance is:";

// Failed results
const char *ERROR_BROKE   = "Transaction failed, insufficient funds";

const char *ERROR_LOADED  = "Transaction failed, amount above daily limit";

const char *ERROR_ACCOUNT = "Invalid account number, try again";
