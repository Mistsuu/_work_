#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>       // memset()
#include <unistd.h>       // sleep()
#include <pthread.h>      // pthread_create(), pthread_join()
#include <stdlib.h>       // atoi(), realloc()
#include <signal.h>       // signal()
#include <ncurses.h>      // wprintw(), wscanw(), getch(), initscr(), start_color();
#include <semaphore.h>    // sem_init(), sem_post(), sem_wait(), sem_destroy()
#include <ctype.h>        // isprint()
#include <assert.h>       // assert()

#define DEFAULTPORT 9669
#define BUFSIZE     1024
#define TERMINATE   '\0'
#define COMMAND     0
#define MESSAGE     1
#define NAMELIMIT   16

/* Data for server */
char               hostname[BUFSIZE];
char               username[NAMELIMIT];
char               chatWith[NAMELIMIT];
int                sockfd;
struct sockaddr_in saddr;
unsigned short     port;
int                listlen=0;

/* Windows for special terminal */
WINDOW *title;
WINDOW *inputWindow;
WINDOW *spaceWindow;
WINDOW *outputWindow;
WINDOW *meWindow;
WINDOW *Border;

/* Screen size */
int height, width;

/* A lock to prevent multiple closeConnection() call */
int closing=0;

/* Specify if the user is in chat mode */
int   chatMode=0;
char  chatUser[NAMELIMIT];

/* List of functions in the code (except main()) */
void start_ncurses();
void createWindows();
int inputString(char** pbuf);
void printUsage();
int getHostname(int argc, char** argv);
int getUsername();
int setup();
void closeConnection();
void displayHelp();
int initChatWith(char *chatWith);
void quitChat();
void getUserList();
int inputType(char* buf, int bufsize);
void executeCommand(char* cmd);
void sendToServer(char* buf, long int size);
void* handleSend();
char* recvFromServer();
void* handleRecv();
int connectToServer();
void handleConnection();

/////////////////////////////////////////////////////// TERMINAL CONFIG ///////////////////////////////////////////////////////////

/*
    start_ncurses():
        * Function: Start special terminal mode
*/
void start_ncurses() {
    // Init screen and start color mode
    initscr();
    start_color();

    // no_echo to the screen
    noecho();

    // define color pairs (for later use)
    init_pair(1, COLOR_BLACK, COLOR_WHITE);
    init_pair(2, COLOR_WHITE, COLOR_CYAN);

    // get screen size
    getmaxyx(stdscr, height, width);
}

/*
    createWindows():
        * Function: Create smaller 'windows' for terminal
*/
void createWindows() {
    /////////////////////////////////////////// TITLE ////////////////////////////////////////////
    // create new win
    title = newwin(1, width, 0, 0);
    wbkgd(title, COLOR_PAIR(1));        

    // print title and refresh
    mvwprintw(title, 0, width/2-3, "Client");
    wrefresh(title);

    ///////////////////////////////// BORDER & OUTPUT WINDOW /////////////////////////////////////
    // create new win - for border thing
    Border = newwin(height - 2, width, 1, 0);

    // create box around window
    box(Border, 0, ' ');
    wrefresh(Border);

    // create another new win
    outputWindow = newwin(height-4, width-2, 2, 1);
    scrollok(outputWindow, TRUE);

    /////////////////////////////////////////// CHAT /////////////////////////////////////////////
    // create win that outputs "[$username$]>"
    meWindow = newwin(1, strlen(username)+3, height-1, 0);
    wbkgd(meWindow, COLOR_PAIR(2));
    wprintw(meWindow, "[%s]> ", username);
    wrefresh(meWindow);

    // create new win that is 1x1 to look nice
    spaceWindow = newwin(1, 1, height-1, strlen(username)+3);
    wbkgd(spaceWindow, COLOR_PAIR(1));
    scrollok(spaceWindow, TRUE);
    wrefresh(spaceWindow);

    // create new win that outputs your input
    inputWindow = newwin(1, width-strlen(username)-4, height-1, strlen(username)+4);
    wbkgd(inputWindow, COLOR_PAIR(1));
    scrollok(inputWindow, TRUE);
    wrefresh(inputWindow);
}


/////////////////////////////////////////////////////// I/0 HANDLER /////////////////////////////////////////////////////////////

/*
    output():
        * Function: Print to output window
*/
#define output(...) wprintw(outputWindow, __VA_ARGS__), wrefresh(outputWindow)

/*
    clear():
        * Function: Clear the window screen.
*/
#define clearScreen() werase(outputWindow), wrefresh(outputWindow)

/*
    inputString():
        * Function: Input a string and returns the size of the input buffer.
*/
int inputString(char** pbuf) {
    char  c;
    int   bufsize   = BUFSIZE;
    int   inputLen  = 0;
    int   keepInput = 1;
    char* buf       = (char*) malloc(bufsize);

    while (keepInput) {
        // Scan for a character
        c = wgetch(inputWindow);

        if (c == '\n') {            /* If newline, break */
            if (inputLen == 0) continue;
            else             keepInput=0, c=TERMINATE, wprintw(inputWindow, "\n");
        } else if (!isprint(c)) {   /* Skip non printable chracters */
            if (c == 127 && inputLen > 0) {
                if (inputLen % (width - (strlen(username) + 4)) != 0) 
                    wprintw(inputWindow, "\b \b");

                (buf)[inputLen - 1] = TERMINATE;

                if (inputLen % (width - (strlen(username) + 4)) == 0)
                    wprintw(inputWindow, "%s", buf + inputLen - width + (strlen(username) + 4));

                inputLen--;
            }
            continue;
        }

        // Reallocate memory if it's too small
        inputLen++; 
        if (inputLen > bufsize) {
            bufsize += BUFSIZE;
            buf = (char*) realloc(buf, bufsize);
        } 
        
        // Set character to memory
        (buf)[inputLen - 1] = c;

        // print this character out
        wprintw(inputWindow, "%c", c);
    }

    *pbuf = buf;
    return bufsize;
}

///////////////////////////////////////////////////////// SETUP ////////////////////////////////////////////////////

/*
    printUsage():
        * Function: Print how to use the executable
*/
void printUsage() {
    printf("[usage] ./<client> <hostname> <port>\n");
    printf("                                 ^\n");
    printf("                                 |\n");
    printf("                             not compulsory\n");
    printf("                            (default to 8784)\n");
}

/*
    getHostname():
        * Argument: Passed from main()
        * Function: Get hostname & set port
*/
int getHostname(int argc, char** argv) {

    if (argc == 1) {
        // Get hostname (through stdin)
		printf("* Enter the name of the host you want to lookup: "), scanf("%s", hostname);
        // Set port to DEFAULTPORT
        port = DEFAULTPORT;
        return 0;
    }


	else if (argc == 2) {
        // Get hostname
		strcpy(hostname, argv[1]),
        // Set port to DEFAULTPORT
        port = DEFAULTPORT;
        return 0;
    }


    else if (argc == 3) {
        // Get hostname
        strcpy(hostname, argv[1]);
        // Get port
        port = (unsigned short) atoi(argv[2]);
        if (port == 0) {
            printf("[!] This port is not real!!!\n");
            printUsage();
            return -1;
        }
        return 0;
    }


    else {
        printf("[!] <= 3 args man!/woman! (gender equality)\n");
        printUsage();
        return -1;
    }
}

/*
    getUsername():
        * Function: Get username.
*/
int getUsername() {
    char buf[BUFSIZ];
    assert(BUFSIZ > NAMELIMIT);
    
    // Print ask
    printf("[*] What is your username? "); fflush(stdin);
    
    // Input
    scanf("%s", buf);
    if (strlen(buf) >= NAMELIMIT) {
        printf("[!] Please input your fucking name under 16 characters!\n");
        return -1;
    }

    // Set username to variable
    memset(username, 0,   NAMELIMIT);
    memcpy(username, buf, NAMELIMIT);

    // Yey ok
    return 0;
}

/*
    setup():
        * Function: Setup server's data for us to connect to
*/
int setup() {
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    // Set address
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_port   = htons(port);
    saddr.sin_family = AF_INET;
    if (inet_pton(AF_INET, hostname, &saddr.sin_addr) <= 0) {
        printf("[!] What the hell address you give me?\n");
        return -1;
    }

    return 0;
}

///////////////////////////////////////////// CLOSING CONNECTION /////////////////////////////////////////////////////////

/*
    closeConnection():
        * Function: Closing connection with server and exit
*/
void closeConnection() {
    // Lock if there're multiple closeConnection() calls...
    if (closing)
        return;
    closing = 1;

    output("[*] Disconnected from server...\n");

    // Shutting down...
    shutdown(sockfd, SHUT_RDWR);
    // Any last words?
    char c; while ((read(sockfd, &c, sizeof(c))) > 0);
    // Closing socket
    close(sockfd);

    // Wait for me? Please?
    output("* Press ANY KEY to exit. *\n");
    wgetch(outputWindow);

    // Clean windows
    endwin();
    exit(0);
}

/////////////////////////////////////////////// FUNCTIONALITIES /////////////////////////////////////////////////

/* 
    displayHelp():
        Display help message.
*/
void displayHelp() {
    output("[?] Using command eh?\n");
    output("    /help            : Display this help.\n");
    output("    /chat <username> : Chat with an user.\n");
    output("    /list            : List users currently active in the server.\n");
    output("    /clear           : Clear screen.\n");
    output("    /quit            : (while not in chat) Quit the server.\n");
    output("                       (while in chat)     Quit chatting with a person.\n");
}

/* 
    initChatWith():
        Initiate chat with an username.
*/
int initChatWith(char *chatWith) {
    // Check name length
    if (strlen(chatWith) >= NAMELIMIT) {
        output("[!] You just inputted an impossible name!\n");
        return -1;
    }

    // Check if you're chatting with yourself...
    if (strcmp(chatWith, username) == 0) {
        output("[?] Why the hell do you want to chat with yourself...\n");
        return -1;
    }

    // Clear screen
    clearScreen();

    // Copy name to chat user...
    memset(chatUser, 0, NAMELIMIT);
    strcpy(chatUser, chatWith);

    // Output status
    output("[*] Chatting with %s...\n", chatWith);

    // Set mode to 1
    chatMode = 1;

    return 0;
}

/* 
    quitChat():
        Quit chatting with a person.
*/
void quitChat() {
    output("[*] Quit chatting with %s...\n", chatUser);
    chatMode = 0;
}

/* 
    getUserList():
        Get current active current user list in the server.
*/
void getUserList() {
    sendToServer("/list", strlen("/list")+1);
}

/*
    login():
        Login to server
*/
int login(char* _username_) {
    // Create bigger buffer to store command
    char* loginBuffer = malloc(strlen("/login ")+strlen(_username_)+1);
    memcpy(loginBuffer, "/login ", strlen("/login "));
    memcpy(&loginBuffer[strlen("/login ")], _username_, strlen(_username_));
    loginBuffer[strlen("/login ")+strlen(_username_)] = '\0';

    // Send login buffer to server
    sendToServer(loginBuffer, strlen("/login ")+strlen(_username_)+1);

    // Receive if the status is OK, then proceed with the code...
    char* status = recvFromServer();
    if (strcmp(status, "ok") != 0) {
        printf("[!] Sorry... the username has been taken!\n");
        return -1;
    }
    free(status);

    return 0;
}

////////////////////////////////////////////// COMMAND PARSER //////////////////////////////////////////////////

/*
    inputType():
        * Function: Specify the type of the buffer
        * (is it a command or something to send to the server?)
*/
int inputType(char* buf, int bufsize) {
    int  i = 0;
    while (i < bufsize && buf[i] == ' ')
        i++;
    if (i < bufsize && buf[i] == '/')
        return COMMAND;
    return MESSAGE;
}

/*
    executeCommand():
        * Function: Execute a command on the client.
*/
void executeCommand(char* cmd) {
    output("\n------------------ %s ------------------\n", cmd);
    char* startCmdPtr;

    /*
        /quit
    */
    if ( ((startCmdPtr = strstr(cmd, "/quit ")) != NULL) || ((startCmdPtr = strstr(cmd, "/quit\0")) != NULL) ) {
        if (!chatMode)
            closeConnection();
        else {
            quitChat();
            sendToServer(startCmdPtr, strlen(startCmdPtr)+1);
        }
    }

    /*
        /chat <username>
    */
    else if ( ((startCmdPtr = strstr(cmd, "/chat ")) != NULL)) {
        if (chatMode)
            output("[!] You are still chatting with %s. Please /quit the current person first!\n", chatUser);
        else if (initChatWith(&startCmdPtr[strlen("/chat ")]) >= 0)
            sendToServer(startCmdPtr, strlen(startCmdPtr)+1);
    }

    /*
        /help
    */
    else if ( ((startCmdPtr = strstr(cmd, "/help ")) != NULL) || ((startCmdPtr = strstr(cmd, "/help\0")) != NULL) ) {
        displayHelp();
    }

    /*
        /list
    */
    else if ( ((startCmdPtr = strstr(cmd, "/list ")) != NULL) || ((startCmdPtr = strstr(cmd, "/list\0")) != NULL) ) {
        getUserList();
    }

    /*
        /clear
    */
    else if ( ((startCmdPtr = strstr(cmd, "/clear ")) != NULL) || ((startCmdPtr = strstr(cmd, "/clear\0")) != NULL) ) {
        clearScreen();
    }

    output("\n");
}

/////////////////////////////////////////////// SENDING  /////////////////////////////////////////////////

/*
    sendToServer():
        * Function: Send data to server by sending size following by content.
*/
void sendToServer(char* buf, long int size) {
    send(sockfd, &size, sizeof(long int), 0);
    send(sockfd, buf, size, 0);
}

/*
    handleSend(): <Sending thread>
        * Function: Sending data to the server
*/
void* handleSend() {
    char* buf = NULL;
    while (1) {
        int bufsize = inputString(&buf);
        if (!buf)  
            output("[!] Error input!\n");

        // If the input is a command
        else if (inputType(buf, bufsize) == COMMAND)
            executeCommand(buf);

        // If the input is input :>
        else {
            if (chatMode) {
                output("[me]> %s\n", buf),
                sendToServer(buf, bufsize);
            } else {
                output("[!] You are not in chat mode. Please choose a user to chat with by using /chat <username>.\n");
            }
        }

        // Free allocated memory to save the environment
        free(buf);
    }
}

//////////////////////////////////////////////////////// RECEIVING /////////////////////////////////////////////////////////////

/*
    recvFromServer(): 
        * Function: Receive data from the server cleanly. 
                    Returns a dynamic buffer.
*/
char* recvFromServer() {
    char tmp[BUFSIZE];  
    memset(tmp, 0, BUFSIZE);

    // Goodbye a client... (sad story)
    if (recv(sockfd, tmp, sizeof(long int), 0) <= 0) {
        closeConnection();
        return NULL;
    }

    // Receive the number of bytes the client receives.
    int   bytesRemaining = *(long int*)&tmp[0];
    int   bytesReceived  = 0;
    int   totalBytes     = 0;
    char* buf            = (char*) malloc(0);

    // Check data size
    if (bytesRemaining >= 0) {
        // output("[*] Incoming %d bytes from server...\n", bytesRemaining);
    } else {
        // output("[*] Invalid data size: %d.\n", bytesRemaining);
        free(buf);
        return NULL;
    }

    // Receiving...
    while (bytesRemaining > 0) {
        // Data breakdown in the middle of transaction...
        if ((bytesReceived = recv(sockfd, tmp, (BUFSIZE < bytesRemaining ? BUFSIZE : bytesRemaining), 0)) <= 0) {
            free(buf);
            closeConnection();
            return NULL;
        }

        // Updates the bytes remaining...
        bytesRemaining -= bytesReceived;
        totalBytes     += bytesReceived;

        // Updates the buffer.
        buf = (char*) realloc(buf, totalBytes);
        memcpy(&buf[totalBytes - bytesReceived], tmp, bytesReceived);
    }

    return buf;
}

/*
    handleRecv(): <Receiving thread>
        * Function: Handle receiving data from server
*/
void* handleRecv() {
    char* buf;
    while (!closing)
        if (buf = recvFromServer()) {
            // Output that data
            output("%s\n", buf);
            // Free the memory
            free(buf);
        }
}

/////////////////////////////////////////////////////// CONNECT TO SERVER /////////////////////////////////////////////////////////

void handleConnection() {
    pthread_t recvThread, sendThread;

    pthread_create(&recvThread, NULL, handleRecv, NULL);
    pthread_create(&sendThread, NULL, handleSend, NULL);

    pthread_join(sendThread, NULL);
    pthread_join(recvThread, NULL);
}

/*
    connectToServer():
        * Function: connecting to the IP by hostname
*/
int connectToServer() {
    // Try connecting to that address
    if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
        printf("[!] Unsuccessful connection.\n");
        return -1;
    }

    // Send username to server
    if (login(username) < 0) {
        return -1;
    }

    return 0;
}

////////////////////////////////////////////////////////////// MAIN ////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
	// Get hostname 
	if (getHostname(argc, argv) < 0)
        return -1;

    // Get username
    if (getUsername() < 0)
        return -1;

    // Setup the server address
    if (setup() < 0)
        return -1;

    // Connect to the server
    if (connectToServer() < 0)
        return -1;

    // Start graphical stuff
    start_ncurses();
    createWindows();

    // Print welcome message
    output("[*] You are in the server. Press /help for help menu.\n");

    // Handle connection from server
    handleConnection();
}
