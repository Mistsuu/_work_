#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>        // printf()
#include <string.h>       // memset()
#include <arpa/inet.h>    // close()
#include <unistd.h>       // close()
#include <errno.h>
#include <sys/time.h>     // FD_SET(), FD_ISSET(), FD_ZERO(), FD_CLR()
#include <stdlib.h>       // atoi()
#include <sys/select.h>   // select()
#include <dirent.h>       // struct dirent, opendir(), closedir()
#include <assert.h>       // assert()
#include <sys/stat.h>     // stat()

#define DEFAULTPORT 8784
#define maxClients  30
#define BUFSIZE     1024

/* Server data */
int                serverfd;
struct sockaddr_in saddr;
unsigned short     port;
/* Client data */
int                clientlen;
int                clientfds [maxClients];
struct sockaddr_in caddrs    [maxClients];

void printUsage();
int getPort(int argc, char** argv);
int setupServer();
int setupClient();
long int getFileSize(FILE* fileptr);
void recvFileFromCli(int i, char* filename);
void sendFileToCli(int i, char* filename);
void listFilesToCli(int i);
void sendToCli(int i, char* buf, long int size);
char* recvFromCli(int i);
void* handleRecv();
void startServer();

///////////////////////////////////////////////////////// SETUP ////////////////////////////////////////////////////

/*
    printUsage():
        * Function: Print how to use the executable
*/
void printUsage() {
    printf("[usage] ./<whateverthisnameis> <portnum>\n");
    printf("                                   ^\n");
    printf("                                   |\n");
    printf("                               not compulsory\n");
    printf("                              (default to 8784)\n");
}

/*
    getPort():
        * Arguments: argc, argv from main()
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

    printf("[*] Listening on port %d...\n", port);
    return 0;
}

/*
    setupClient():
        * Function: Set up variables for client(s)
*/
int setupClient() {
    clientlen = sizeof(struct sockaddr_in);

    for (int i = 0; i < maxClients; ++i) {
        clientfds[i]  = 0;
    }
}

///////////////////////////////////////////////////////// CLOSE ////////////////////////////////////////////////////
void closeConnectionCli(int i) {
    printf("[!] Goodbye baby [%s:%d]... (huhu)... :(\n", inet_ntoa(caddrs[i].sin_addr), ntohs(caddrs[i].sin_port));
    close(clientfds[i]);
    clientfds[i] = 0;
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
    recvFileFromCli():
        * Function: Write file content from client.
*/
void recvFileFromCli(int i, char* filename) {
    printf("[i] Receiving an upload request from client %d... Receiving %s\n", i, filename);

    // Get file data
    FILE* file          = fopen(filename, "wb");
    if (!file)
        printf("[!] Cannot write to file!\n");
    int   bytesReceived = 0;

    // Getting file size
    char filesizebuf[sizeof(long int)];
    if (recv(clientfds[i], filesizebuf, sizeof(long int), 0) <= 0) {
        closeConnectionCli(i);
        return;
    }

    // Checking file size
    long int filesize = *(long int*) &filesizebuf;
    printf("[i] Incoming file size: %ld\n", filesize);
    if (filesize == -1) {
        printf("[!] Download a file that does not exist!\n");
        return;
    }

    // Read content from buffer
    printf("[i] Receiving...\n");
    char buf[BUFSIZE]; 
    while (filesize > 0) {
        // Incase the client bails
        if ((bytesReceived = recv(clientfds[i], buf, BUFSIZE, 0)) <= 0) {
            closeConnectionCli(i);
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
    sendFileToCli():
        * Function: Send file content to the client.
*/
void sendFileToCli(int i, char* filename) {
    printf("[i] Receiving a download request from client %d... Getting %s\n", i, filename);

    // Check if file is openable...
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("[!] The file %s does not exists!\n", filename);

        // Sends to inform negative one byte, means invalid
        long int invalid_size = -1;
        send(clientfds[i], &invalid_size, sizeof(long int), 0);

        return;
    }

    // Check if requested file is regular...
    if (!isRegularFile(filename)) {
        printf("[*] Client requests a non-regular file! Name: %s\n", filename);

        // Sends to inform negative one byte, means invalid
        long int invalid_size = -1;
        send(clientfds[i], &invalid_size, sizeof(long int), 0);

        fclose(file);
        return;
    }

    int      bytesRead;
    long int filesize = getFileSize(file);
    char     buf[BUFSIZE];

    // Sends file size to the client.
    printf("[i] Sending file size: %ld\n", filesize);
    send(clientfds[i], &filesize, sizeof(long int), 0);

    // Send content to client
    printf("[i] Sending file's content...\n");
    while (filesize > 0) {
        bytesRead = fread(buf, sizeof(char), BUFSIZE, file);
        send(clientfds[i], buf, bytesRead, 0);
        filesize -= bytesRead;

        // printf("[i] %d bytes sent, %ld bytes remaining...\n", bytesRead, filesize);
    }
    printf("[i] Finishing sending file.\n");

    // Free dynamic content
    fclose(file);
}

/*
    listFilesToCli():
        * Function: List files in the storage directory.
*/
void listFilesToCli(int i) {
    printf("[i] Listing files to client %d...\n", i);

    DIR *dir = opendir(".");
    struct dirent *ent;
    if (dir)
        while ((ent = readdir (dir))) 
            if (ent->d_type == DT_REG)
                sendToCli(i, ent->d_name, strlen(ent->d_name)+1);

    // Sends negative one file size to signify the end of data
    long int invalid_size = -1;
    send(clientfds[i], &invalid_size, sizeof(long int), 0);
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

///////////////////////////////////////////// RECEIVING  ///////////////////////////////////////////////

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
        closeConnectionCli(i);
        return NULL;
    }

    // Receive the number of bytes the client receives.
    int   bytesRemaining = *(long int*)&tmp[0];
    int   bytesReceived  = 0;
    int   totalBytes     = 0;
    char* buf            = (char*) malloc(0);
    
    // Check data size
    if (bytesRemaining >= 0) {
        // printf("[*] Incoming %d bytes from client %d...\n", bytesRemaining, i);
    } else {
        // printf("[*] Invalid data size: %d.\n", bytesRemaining);
        free(buf);
        return NULL;
    }

    while (bytesRemaining > 0) {
        // Data breakdown in the middle of transaction...
        if ((bytesReceived = recv(clientfds[i], tmp, (BUFSIZE < bytesRemaining ? BUFSIZE : bytesRemaining), 0)) <= 0) {
            free(buf);
            closeConnectionCli(i);
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
    handleRecv():
        * Function: Handle receiving data from client (and upcoming connections)
*/
void* handleRecv() {
    // Define variables for client & socket set
    struct sockaddr_in caddr;
    int                clientfd;
    fd_set             readfds;
    int                maxfd;

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
                printf("[!] Receving a connection from client, but error upon receiving clientfd...\n");
            
            else {
                printf("[*] Hello baby [%s:%d]! Welcome to my house!\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
                for (int i = 0; i < maxClients; ++i)
                    if (clientfds[i] == 0) {
                        clientfds[i] = clientfd;
                        caddrs   [i] = caddr;
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
                // Get command from client.
                char* cmd = recvFromCli(i);
                char* startCmdPtr;

                // Yey someone send us something, let's unpack it!
                if (cmd) {

                    // Download
                    if ( ((startCmdPtr = strstr(cmd, "download ")) != NULL) )
                        sendFileToCli(i, &startCmdPtr[strlen("download ")]);

                    // Upload
                    else if ( ((startCmdPtr = strstr(cmd, "upload ")) != NULL) )
                        recvFileFromCli(i, &startCmdPtr[strlen("upload ")]);

                    // List
                    else if ( ((startCmdPtr = strstr(cmd, "list ")) != NULL) || ((startCmdPtr = strstr(cmd, "list\0")) != NULL) )
                        listFilesToCli(i);

                    
                    // Deallocate the pointer to save memory.
                    free(cmd);
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
    handleRecv();
}

/////////////////////////////////////////////////////// MAIN ///////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
    if (getPort(argc, argv) < 0)
        return -1;

    if (setupServer() < 0)
        return -1;

    setupClient();
    startServer();
}