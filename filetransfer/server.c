// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 1024 

int getPort(char *arr[], int argc) {
    if (argc < 2) return -1;
    int port;
	const char *errstr;

    port =	strtonum(arr[1], 1000, 9999, &errstr);
    if (errstr != NULL) {
        return -1;
    } else {
        return port;
    }
}

// Driver code 
int main(int argc, char *argv[]) { 
    int PORT = getPort(argv, argc);
    if (PORT < 0) {
        printf("Invalid port input.");
        return -1;
    }
    int sockfd; 
    char buffer[MAXLINE]; 
    char *ftp = "ftp";
    char *yes = "yes"; 
    char *no = "no"; 
    struct sockaddr_in servaddr, cliaddr; 

    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    int len, n; 
  
    len = sizeof(cliaddr);
  
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
                &len); 
    buffer[n] = '\0'; 

    int is_valid = strcmp("ftp", &buffer);
    char *response;
    if (is_valid != 0) {
        response = no;
    } else {
        response = yes;
    }
    
    printf("&s", buffer);
    sendto(sockfd, (const char *)response, strlen(response),  
        NULL, (const struct sockaddr *) &cliaddr, 
            len);

    return 0; 
}