#include <sys/socket.h>
#include <sys/stat.h>     // mkdir(), stat()
#include <netinet/in.h>
#include <stdio.h>        // printf()
#include <string.h>       // memset()
#include <arpa/inet.h>    // close()
#include <unistd.h>       // close()
#include <pthread.h>      // pthread_create(), pthread_join()
#include <errno.h>
#include <sys/time.h>     // FD_SET(), FD_ISSET(), FD_ZERO(), FD_CLR()
#include <stdlib.h>       // atoi()
#include <sys/select.h>   // select()
#include <ncurses.h>      // wprintw(), wscanw(), getch(), initscr(), start_color();
#include <ctype.h>        // isprint()

#define maxClients  1000
#define BUFSIZE     1024
#define COMMAND     0
#define MESSAGE     1
#define TERMINATE   '\0'
#define DEFAULTPORT 9669
#define NAMELIMIT   16

/* Server data */
int                serverfd;
struct sockaddr_in saddr;
unsigned short     port;

/* Client data */
int                clientlen;
int                clientfds      [maxClients];
struct sockaddr_in caddrs         [maxClients];
char               username       [maxClients][NAMELIMIT];
int                isChatCli      [maxClients];
char               chatUserCli    [maxClients][NAMELIMIT];
int                cliRegisterYet [maxClients];

/* Windows for special terminal */
WINDOW *title;
WINDOW *inputWindow;
WINDOW *outputWindow;
WINDOW *meWindow;
WINDOW *Border;

/* Screen size */
int height, width;

/* List of functions in the code (except main()) */
void _check_and_make_dir(char* dirname);
long int getFileSize(FILE* fileptr);
void printUsage();
int getPort(int argc, char** argv);
int setupServer();
int setupClient();
void goodbyeClient(int i);
void initChatCli(int i, char* userTo);
void quitChatCli(int i);
void listUsersToCli(int i);
void loginCli(int i, char* _username_);
void checkStorage(char* _username_);
void storeConversationCli(int i, char* content);
void forwardDataCli(int i, char* content);
int inputType(char* buf, int bufsize);
void executeCommandOfCli(int i, char* cmd);
void sendToCli(int i, char* buf, long int size);
char* recvFromCli(int i);
void* handleRecv();
void startServer();

/////////////////////////////////////////////////////// OUTPUT HANDLER /////////////////////////////////////////////////////////////

/*
    output():
        * Function: Print to output window
*/
#define output(...) printf(__VA_ARGS__)

//////////////////////////////////////////////////// FILE OPS /////////////////////////////////////////////////////
/*
    _check_and_make_dir():
        * Function: Create a directory if there has not exist such one.
*/
void _check_and_make_dir(char* dirname) {
    struct stat st = {0};
    if (stat(dirname, &st) == -1)
        mkdir(dirname, 0700);
}

/*
    getFileSize():
        * Function: Get file size.
*/
long int getFileSize(FILE* fileptr) {
    fseek(fileptr, 0, SEEK_END);
    long int filesize = ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);
    return filesize;
}

///////////////////////////////////////////////////////// SETUP ////////////////////////////////////////////////////
/*
    printUsage():
        * Function: Print how to use the executable
*/
void printUsage() {
    printf("[usage] ./server <portnum>\n");
    printf("                    ^\n");
    printf("                    |\n");
    printf("                not compulsory\n");
    printf("               (default to 8784)\n");
}


/*
    getPort():
        * Arguments: Passed from the main() func
        * Function: Getting port from argument
*/
int getPort(int argc, char** argv) {
    if (argc > 2) {
        printf("[!] <= 2 arguments or not man/woman!\n");
        printUsage();
        return -1;
    }

    if (argc == 1) {
        port = DEFAULTPORT;
        return 0;
    }

    port = (unsigned short) atoi(argv[1]);
    if (port == 0) {
        printf("[!] Not a valid number for port!\n");
        printUsage();
        return -1;
    }

    return 0;
}

/*
    setupServer():
        * Function: Setup variables for the server
*/
int setupServer() {
    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family      = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port        = htons(port);

    int opt = 1;
    if (setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {   
        printf("[!] Unable to allow reusing address!\n");
        return -1;
    } 

    if (bind(serverfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
        printf("[!] Cannot bind!\n");
        return -1;
    }

    if (listen(serverfd, maxClients) < 0) {
        printf("[!] Cannot listen!\n");
        return -1;
    }  

    return 0;
}

/*
    setupClient():
        * Function: Set up variables for client(s)
*/
int setupClient() {
    clientlen = sizeof(struct sockaddr_in);

    for (int i = 0; i < maxClients; ++i)
        clientfds[i]      = 0,
        isChatCli[i]      = 0,
        cliRegisterYet[i] = 0;
}

////////////////////////////////////////////// DISCONNECT ////////////////////////////////////////////////

/*
    goodbyeClient():
        * Function: Bye a client!
*/
void goodbyeClient(int i) {
    output("[!] Goodbye baby [%s:%d]... (huhu)... :(\n", inet_ntoa(caddrs[i].sin_addr), ntohs(caddrs[i].sin_port));
    // Shutting down
    shutdown(clientfds[i], SHUT_RDWR);
    // Receive possible remaining data
    char c; while ((read(clientfds[i], &c, sizeof(c))) > 0);
    // Closing socket
    close(clientfds[i]);
    // Setting one of the client sockets and states to 0
    clientfds[i] = 0;
    cliRegisterYet[i] = 0;
}

/////////////////////////////////////////////// FUNCTIONALITIES /////////////////////////////////////////////////

/* 
    initChatCli():
        * Function: Intiate a chat of a client to another client.
*/
void initChatCli(int i, char* userTo) {
    output("[*] Client \"%s\" is requesting to chat with \"%s\"...\n", username[i], userTo);

    // Copy username the person is chatting to
    memset(chatUserCli[i], 0, NAMELIMIT);
    strcpy(chatUserCli[i], userTo);

    // Set chat state to 1
    isChatCli[i] = 1;

    // Create variable for name storage
    char* unreadFilename = malloc(strlen("storage/") + strlen(username[i]) + strlen("/") + strlen(userTo) + 1);
    // Add NULL terminate
    unreadFilename[strlen("storage/") + strlen(username[i]) + strlen("/") + strlen(userTo)] = '\0';
    // Write to variable
    sprintf(unreadFilename, "storage/%s/%s", username[i], userTo);

    // Check if there is any message from storage
    FILE *unreadFile = fopen(unreadFilename, "r");
    if (unreadFile) {
        // Init file size & array containing data
        long int  fileSize      = getFileSize(unreadFile);
        char*     unreadContent = malloc(sizeof(char) * fileSize);
        // Read content from file & send
        fread(unreadContent, fileSize, sizeof(char), unreadFile);
        sendToCli(i, unreadContent, fileSize);
        // Close file
        fclose(unreadFile);
        // Remove file
        remove(unreadFilename);
    }

    // Free dynamic content
    free(unreadFilename);
}

/*
    quitChatCli():
        * Function: Remove chatting state of a client.
*/
void quitChatCli(int i) {
    output("[*] Client \"%s\" is requesting to quit chat with \"%s\"...\n", username[i], chatUserCli[i]);
    isChatCli[i] = 0;
}

/*
    listUsersToCli():
        * Function: List active users to a client.
*/
void listUsersToCli(int i) {
    output("[*] Client \"%s\" is requesting to list usernames...\n", username[i]);

    // Send usernames to client
    for (int j = 0; j < maxClients; ++j)
        if (clientfds[j] != 0)
            sendToCli(i, username[j], strlen(username[j])+1);

    // Sends to inform negative one byte, means end of data
    sendToCli(i, "\n", 2);
}

/*
    checkStorage():
        * Function: Check if the storage folder belongs to an user is created.
*/
void checkStorage(char* _username_) {
    // Create variable for name storage
    char* storageDirname = malloc(strlen("storage/") + strlen(_username_) + 1);
    // Add NULL terminate
    storageDirname[strlen("storage/") + strlen(_username_)] = '\0';
    // Write to variable
    sprintf(storageDirname, "storage/%s", _username_);
    // Create directory if it does not exist
    _check_and_make_dir(storageDirname);
    // Free dynamic content
    free(storageDirname);
}

/*
    storeConversationCli():
        * Function: Stores the content of an user to another one
                    in case of that other one is offline.
*/
void storeConversationCli(int i, char* content) {
    // Create variable for name storage
    char* storage_filename = malloc(strlen("storage/") + strlen(chatUserCli[i]) + strlen("/") + strlen(username[i]) + 1);
    // Add NULL terminate
    storage_filename[strlen("storage/") + strlen(chatUserCli[i]) + strlen("/") + strlen(username[i])] = '\0';
    // Write to variable
    sprintf(storage_filename, "storage/%s/%s", chatUserCli[i], username[i]);
    // Open file for storage
    FILE *content_file = fopen(storage_filename, "a");
    fwrite(content, sizeof(char), strlen(content), content_file);
    fwrite("\n",    sizeof(char), 1,               content_file);
    // Free dynamic content
    free(storage_filename);
    fclose(content_file);
}

/*
    forwardDataCli():
        * Function: Forwards data if the two clients are talking to
                    each other at the moment. Store content to a file
                    if they're not.
*/
void forwardDataCli(int i, char* content) {
    // Create memory for the content we need to send
    char* sendContent = malloc(strlen(content) + strlen(username[i]) + strlen("[]> ") + 1);
    // Set NULL terminate
    sendContent[strlen(content) + strlen(username[i]) + strlen("[]> ")] = '\0';
    // Write to sendContent variable
    sprintf(sendContent, "[%s]> %s", username[i], content);

    // Find the other client
    int isOtherClientExist = 0;
    for (int j = 0; j < maxClients; ++j) {
        if (j != i && clientfds[j] && isChatCli[j])
            // If this is a two-way chat
            if (strcmp(username[j], chatUserCli[i]) == 0 && strcmp(username[i], chatUserCli[j]) == 0) {
                sendToCli(j, sendContent, strlen(sendContent)+1);
                isOtherClientExist = 1;
                break;
            }
    }

    // Write to disk if other client is not visible...
    if (!isOtherClientExist) {
        checkStorage(chatUserCli[i]);
        storeConversationCli(i, sendContent);
    }

    // Free dynamic memory
    free(sendContent);
}

/*
    loginCli():
        * Function: Login to client.
*/
void loginCli(int i, char* _username_) {

    // Check if user name is taken
    int isUsernameTaken = 0;
    for (int j = 0; j < maxClients; ++j)
        if (i != j && clientfds[j] && strcmp(username[j], _username_) == 0) {
            isUsernameTaken = 1;
            break;
        }

    // Remove client if the username has been taken...
    if (isUsernameTaken) {
        output("[*] (%s:%d) registers for username \"%s\", but that username has been taken...\n", inet_ntoa(caddrs[i].sin_addr), ntohs(caddrs[i].sin_port), _username_);
        sendToCli(i, "no", strlen("no")+1);
        clientfds[i] = 0; 
    } 
    else {
        // Set username in memory
        memset(username[i], 0,   NAMELIMIT);
        memcpy(username[i], _username_, strlen(_username_));
        output("[*] (%s:%d) registers for the name \"%s\" successfully!\n", inet_ntoa(caddrs[i].sin_addr), ntohs(caddrs[i].sin_port), _username_);
        sendToCli(i, "ok", strlen("ok")+1);
    }

    // Set state of client to registered
    cliRegisterYet[i] = 1;

}

//////////////////////////////////////////// COMMAND PARSER ////////////////////////////////////////////////////

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
    executeCommandOfCli():
        * Function: Execute a command of a the client.
*/
void executeCommandOfCli(int i, char* cmd) {
    char* startCmdPtr;


    if (cliRegisterYet[i]) {
        /*
            /chat <username>
        */
        if ( ((startCmdPtr = strstr(cmd, "/chat ")) != NULL)) {
            initChatCli(i, &startCmdPtr[strlen("/chat ")]);
        }
    }

    /*
        /list
    */
    if ( ((startCmdPtr = strstr(cmd, "/list ")) != NULL) || ((startCmdPtr = strstr(cmd, "/list\0")) != NULL) ) {
        listUsersToCli(i);
    }

    /*
        /quit
    */
    if ( ((startCmdPtr = strstr(cmd, "/quit ")) != NULL) || ((startCmdPtr = strstr(cmd, "/quit\0")) != NULL) ) {
        quitChatCli(i);
    }

    /*
        /login <username>
    */
    if ( ((startCmdPtr = strstr(cmd, "/login ")) != NULL) || ((startCmdPtr = strstr(cmd, "/login\0")) != NULL) ) {
        loginCli(i, &startCmdPtr[strlen("/login ")]);
    }
}

///////////////////////////////////////////// SENDING  ///////////////////////////////////////////////

/*
    sendToCli(): 
        * Function: Send data to a client. 
                    By sending size following by content.
*/
void sendToCli(int i, char* buf, long int size) {
    send(clientfds[i], &size, sizeof(long int), 0);
    send(clientfds[i], buf, size, 0);
}

//////////////////////////////////////////////////////// RECEIVING /////////////////////////////////////////////////////////////

/*
    recvFromCli(): 
        * Function: Receive data from a client cleanly. 
                    Returns a dynamic buffer.
*/
char* recvFromCli(int i) {
    char tmp[BUFSIZE];  
    memset(tmp, 0, BUFSIZE);

    // Goodbye a client... (sad story)
    if (recv(clientfds[i], tmp, sizeof(long int), 0) <= 0) {
        goodbyeClient(i);
        return NULL;
    }

    // Receive the number of bytes the client receives.
    int   bytesRemaining = *(long int*)&tmp[0];
    int   bytesReceived  = 0;
    int   totalBytes     = 0;
    char* buf            = (char*) malloc(0);
    
    // Check data size
    if (bytesRemaining >= 0) {
        output("[*] Incoming %d bytes from client %d...\n", bytesRemaining, i);
    } else {
        output("[*] Invalid data size: %d.\n", bytesRemaining);
        free(buf);
        return NULL;
    }

    while (bytesRemaining > 0) {
        // Data breakdown in the middle of transaction...
        if ((bytesReceived = recv(clientfds[i], tmp, (BUFSIZE < bytesRemaining ? BUFSIZE : bytesRemaining), 0)) <= 0) {
            free(buf);
            goodbyeClient(i);
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

/////////////////////////////////////////////////////  SERVER /////////////////////////////////////////////////////////////

/*
    handleRecv():
        * Function: Handle receiving data from client (and upcoming connections)
*/
void* handleRecv() {

    // Define variables for client & socket set
    struct sockaddr_in caddr;
    int                clientfd;
    fd_set             readfds;
    int                maxfd;
    char*              buf;

    while(1) {
        /* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  */

        // Clear the fd(in this case sockets) set
        FD_ZERO(&readfds);

        // Then add them (Again)
        FD_SET(serverfd, &readfds);
        maxfd = serverfd;

        for (int i = 0; i < maxClients; ++i) {
            if (clientfds[i] > 0)
                FD_SET(clientfds[i], &readfds);

            if (clientfds[i] > maxfd)
                maxfd = clientfds[i];
        }

        // Waiting for any activity on one of the fd(in this case sockets) on readfds
        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        /* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  */

        //  If there's acitivity on server,
        //  it only means that we have a new
        //  client connected to!

        if (FD_ISSET(serverfd, &readfds)) {
            if ((clientfd = accept(serverfd, (struct sockaddr*) &caddr, &clientlen)) < 0)
                output("[!] Receving a connection from client, but error upon receiving clientfd...\n");
            else {
                for (int i = 0; i < maxClients; ++i)
                    if (clientfds[i] == 0) {
                        // Set client fd in the socket array
                        clientfds[i] = clientfd;
                        caddrs   [i] = caddr;

                        output("[*] Hello baby (%s:%d)! Welcome to my house!\n", inet_ntoa(caddrs[i].sin_addr), ntohs(caddrs[i].sin_port));
                        break;
                    }   
            }         
        }

        /* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  */

        //  If there's activity on client,
        //  it only means that we receive
        //  a message from them!
        //  (Or disconnect...)

        for (int i = 0; i < maxClients; ++i) {
            if (clientfds[i] > 0 && FD_ISSET(clientfds[i], &readfds)) {
                // Get data from client
                buf = recvFromCli(i);
                if (buf) {
                    // Categorize the input from client
                    if (inputType(buf, strlen(buf)) == COMMAND) {
                        output("[*] Get a request from \"%s\" (%s:%d): \"%s\"\n", username[i], inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port), buf);
                        executeCommandOfCli(i, buf);
                    } else {
                        // Output that data
                        output("(%s)->(%s): %s\n", username[i], chatUserCli[i], buf);
                        // Forward data to correct user
                        forwardDataCli(i, buf);
                    }
                    // Free the memory
                    free(buf);
                }
            }
        }

        /* ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////  */
    }
}


/*
    startServer():
        * Function: Starting server...
*/
void startServer() {
    output("[*] Listening on port %d...\n", port);
    handleRecv();
}


/////////////////////////////////////////////////////// MAIN ///////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    // Get port (from stdin or argv)
    if (getPort(argc, argv) < 0)
        return -1;

    // Create directory for storage if it does not exist
    _check_and_make_dir("storage");

    // Set up variables for server & client
    if (setupServer() < 0)
        return -1;
    setupClient();

    // Let's run the server!
    startServer();
}