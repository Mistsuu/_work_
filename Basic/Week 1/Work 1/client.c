#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>       // memset()
#include <unistd.h>
#include <stdlib.h>       // atoi(), realloc()
#include <signal.h>       // signal()
#include <sys/stat.h>     // stat()

#define DEFAULTPORT 8784
#define BUFSIZE     1024
#define TERMINATE   '\0'
#define COMMAND     0
#define MESSAGE     1

char               hostname[BUFSIZE];
int                sockfd;
struct sockaddr_in saddr;
unsigned short     port;
int                listlen=0;

int inputString(char** buf);
void printUsage();
int getHostname(int argc, char** argv);
int setup();
void closeConnection();
long int getFileSize(FILE* fileptr);
void recvFileFromServer(char* filename);
void sendFileToServer(char* filename);
void sendToServer(char* buf, long int size);
char* recvFromServer();
void executeCommand(char* cmd);
void* handleSend();
int connectToServer();

/////////////////////////////////////////////////////// INPUT HANDLER /////////////////////////////////////////////////////////////
/*
    inputString():
        * Function: Input a string and returns an array of allocated chars.
*/
int inputString(char** buf) {
    char c;
    int  bufsize   = BUFSIZE;
    int  strlen    = 0;
    int  keepInput = 1;

    if (*buf)
        free(*buf);
    *buf = (char*) malloc(bufsize);

    while (keepInput) {
        // Scan for a character
        scanf("%c", &c);
        if (c == '\n') { /* If newline, break */
            if (strlen == 0) continue;
            else             keepInput=0, c=TERMINATE;
        }

        // Reallocate memory if it's too small
        strlen++;
        if (strlen > bufsize) {
            bufsize *= 2;
            *buf = (char*) realloc(*buf, bufsize);
        }

        // Set character to memory
        (*buf)[strlen - 1] = c;
    }

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

    // Try connecting to that address
    if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) {
        printf("[!] Unsuccessful connection.\n");
        return -1;
    }

    return 0;
}

///////////////////////////////////////////// CLOSING CONNECTION //////////////////////////////////////////

/*
    closeConnection():
        * Function: Closing connection with server and exit
*/
void closeConnection() {
    printf("[!] Server disconnected. :(\n");
    close(sockfd);
    exit(0);
}

////////////////////////////////////////// FILE OPS //////////////////////////////////////////////////

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

/*
    recvFileFromServer():
        * Function: Write file content from server.
*/
void recvFileFromServer(char* filename) {
    printf("[i] Receiving %s\n", filename);

    // Get file data
    FILE* file = fopen(filename, "wb");
    if (!file)
        printf("[!] Cannot write to file!\n");

    // Getting file size
    char filesizebuf[sizeof(long int)];
    if (recv(sockfd, filesizebuf, sizeof(long int), 0) <= 0) {
        closeConnection();
        return;
    }

    // Checking file size
    long int filesize = *(long int*) &filesizebuf;
    printf("[i] Incoming file size: %ld\n", filesize);
    if (filesize == -1) {
        printf("[!] Getting a file that does not exist!\n");
        return;
    }

    // Read content from buffer
    printf("[i] Receiving...\n");
    char buf[BUFSIZE];
    int bytesReceived = 0;
    while (filesize > 0) {
        // Incase the client bails
        if ((bytesReceived = recv(sockfd, buf, BUFSIZE, 0)) <= 0) {
            closeConnection();
            return;
        }

        // Write content to file
        fwrite(buf, sizeof(char), bytesReceived, file);

        // Update remaining file size
        filesize -= bytesReceived;

        // printf("[i] Received %d bytes, %ld bytes remaining...\n", bytesReceived, filesize);
    }
    printf("[i] Finished receiving.\n");

    // Free dynamic content
    fclose(file);
}

/*
    isRegularFile():
        * Function: Checks if a file is a file.
*/
int isRegularFile(const char *path)
{
    struct stat fileStat;
    stat(path, &fileStat);
    return S_ISREG(fileStat.st_mode);
}

/*
    sendFileToServer():
        * Function: Send file content to the server.
*/
void sendFileToServer(char* filename) {
    printf("[i] Sending %s\n", filename);

    // Check if file is openable...
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("[!] The file %s does not exists!\n", filename);

        // Sends to inform negative one byte, means invalid
        unsigned long int invalid_size = -1;
        send(sockfd, &invalid_size, sizeof(unsigned long int), 0);

        return;
    }

    // Check if requested file is regular...
    if (!isRegularFile(filename)) {
        printf("[*] Oh wow... Trying to upload a non-regular file! Name: %s\n", filename);

        // Sends to inform negative one byte, means invalid
        long int invalid_size = -1;
        send(sockfd, &invalid_size, sizeof(long int), 0);

        fclose(file);
        return;
    }

    int      bytesRead;
    long int filesize = getFileSize(file);
    char     buf[BUFSIZE];

    // Sends file size to the server.
    printf("[i] Sending file size: %ld\n", filesize);
    send(sockfd, &filesize, sizeof(long int), 0);

    // Send content to client
    printf("[i] Sending file's content...\n");
    while (filesize > 0) {
        bytesRead = fread(buf, sizeof(char), BUFSIZE, file);
        send(sockfd, buf, bytesRead, 0);
        filesize -= bytesRead;

        // printf("[i] %d bytes sent, %ld bytes remaining...\n", bytesRead, filesize);
    }
    printf("[i] Finishing sending file.\n");

    // Free dynamic content
    fclose(file);
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

///////////////////////////////////////////// RECEIVING  ///////////////////////////////////////////////

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
        // printf("[*] Incoming %d bytes from server...\n", bytesRemaining);
    } else {
        // printf("[*] Invalid data size: %d.\n", bytesRemaining);
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
    recvListFromServer(): 
        * Function: Receive list of files from server.
*/
void recvListFromServer() {
    char* buf;
    while ((buf = recvFromServer())) {
        printf("%s\n", buf);
        free(buf);
    }
}

///////////////////////////////////////////// COMMAND  ///////////////////////////////////////////////

/*
    executeCommand():
        * Function: Execute a command on the client.
*/
void executeCommand(char* cmd) {
    char* startCmdPtr;

    /*
        quit
    */
    if ( ((startCmdPtr = strstr(cmd, "quit ")) != NULL) || ((startCmdPtr = strstr(cmd, "quit\0")) != NULL) ) {
        printf("[*] Goodbye server... I'm comin' home, I'm comin' home...\n");
        closeConnection();
    }

    /*
        help
    */
    else if ( ((startCmdPtr = strstr(cmd, "help ")) != NULL) || ((startCmdPtr = strstr(cmd, "help\0")) != NULL) ) {
        printf("[?] Using command eh?\n");
        printf("    quit                : Quit the server.\n");
        printf("    help                : Display this help.\n");
        printf("    list                : List the files in server.\n");
        printf("    upload   <filename> : Upload a file to server.\n");
        printf("    download <filename> : Download a file from server.\n");
    }

    /*
        list
    */
    else if ( ((startCmdPtr = strstr(cmd, "list ")) != NULL) || ((startCmdPtr = strstr(cmd, "list\0")) != NULL) ) {
        sendToServer(startCmdPtr, strlen(startCmdPtr) + 1);
        recvListFromServer();
    }

    /*
        upload <filename>
    */
    else if ( ((startCmdPtr = strstr(cmd, "upload ")) != NULL) ) {
        sendToServer(startCmdPtr, strlen(startCmdPtr) + 1);
        sendFileToServer(&startCmdPtr[strlen("upload ")]);
    }

    /*
        download <filename>
    */
    else if ( ((startCmdPtr = strstr(cmd, "download ")) != NULL) ) {
        sendToServer(startCmdPtr, strlen(startCmdPtr) + 1);
        recvFileFromServer(&startCmdPtr[strlen("download ")]);
    }

    printf("\n");
}


/*
    handleSend(): <Sending thread>
        * Function: Sending data to the server
*/
void* handleSend() {
    char* buf = NULL;
    while (1) {
        printf(">> "); fflush(stdout);
        int bufsize = inputString(&buf);
        if (!buf)
            printf("[!] Error input!\n");
        else
            executeCommand(buf);
    }
}


/////////////////////////////////////////////////////// CONNECT TO SERVER /////////////////////////////////////////////////////////

void handleConnection() {
    handleSend();
}

/*
    connectToServer():
        * Function: connecting to the IP by hostname
*/
int connectToServer() {
    handleConnection();
    return 0;
}

////////////////////////////////////////////////////////////// MAIN ////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

	// Get hostname
	if (getHostname(argc, argv) < 0)
        return -1;

    // Setup the server address
    if (setup() < 0)
        return -1;

    // Connect to the server
    if (connectToServer() < 0)
        return -1;
}
