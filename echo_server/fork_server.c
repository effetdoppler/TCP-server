#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <err.h>
#include "echo.h"
#include <signal.h>

int main(int argc, char** argv)
{
    if (argc != 2)
        errx(EXIT_FAILURE, "Usage:\n"
            "Arg 1 = Port number (e.g. 2048)");

    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *result;
    //Use the getaddrinfo(3) function to get a linked list of addresses.
    if (getaddrinfo(NULL, argv[1], &hints, &result) != 0)
        err(EXIT_FAILURE, "server_connection: getaddrinfo()");
    struct addrinfo *rp;
    int sfd;
    for (rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        //If an error occurs, continue with the next address.
        if (sfd == -1)
            continue;
        int value = 1;
        //set SO_REUSEADDR to 1
        if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &value, sizeof(int))==-1)
            err(EXIT_FAILURE, "server_connection: setsocketopt()");
        // Try to bind the socket to the address
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;
        close(sfd);
    }
    //Free the linked list.
    freeaddrinfo(result);
    //Specify that the socket can be used to accept incoming connections
    if (listen(sfd, 5) == -1)
        err(EXIT_FAILURE, "main: listen()");
    //Print a message saying that your server is waiting for connections.
    printf("Waiting for connections...\n");
    while(1)
    {
        int cfd;
        struct sockaddr client_address;
        socklen_t client_address_length = sizeof(struct sockaddr);
        //Wait for connections by using the accept(2) function
        cfd = accept(sfd, &client_address, &client_address_length);
        if (cfd == -1)
            err(EXIT_FAILURE, "main: accept()");
        pid_t pid = fork();
        if (pid > 0)
        {
            close(cfd);
            continue;
        }
        if (pid == 0)
        {   
            close(sfd);
            //Print any message showing that a connection is successful.
            printf("New connection (pid = %i)\n", getpid());
            //Use the file descriptor of this new socket with your echo() function.
            echo(cfd, cfd);
            //Close the two sockets.
            close(cfd);
            printf("Close connection (pid = %i)\n", getpid());
            exit(EXIT_SUCCESS);
            signal(SIGCHLD, SIG_IGN);
        }
        else
        {
            close(cfd);
            close(sfd);
            // Error handling ...
            err(EXIT_FAILURE, "Error: No Child Process is Created");
        }      
    }
    close(sfd);
    return 0;
}