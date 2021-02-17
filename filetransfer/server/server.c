// Server side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define MAXLINE 1000 

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

void parsePacket(char *packet, char **total_frag, char **frag_no, 
                char **size, char **filename, char **frag) 
{
    char *token = strtok(packet, ":"); 
    int c = 0;

    while (token != NULL) { 
        switch (c) {
            case 0:
                *total_frag = token;
            case 1:
                *frag_no = token;
            case 2:
                *size = token;
            case 3:
                *filename = token;
            case 4:
                *frag = token; 
        }
        c += 1;
        token = strtok(NULL, ":"); 
    }
}

//converts single byte string to ascii char
char byteToChar(char byteStr[]) {
    return (char)strtol(byteStr, NULL, 2);
}
//converts byte string of any length (size) to ascii string
char *bytesToString(char *byteStr, unsigned int size) {
    unsigned int charCount = sizeof(char) * (size/8);
    char *result = malloc(charCount);
    for (int i = 0; i < size; i += 8) {
        char tmp[8];
        strncpy(tmp, byteStr + i, 8);
        char character = byteToChar(tmp);
        strncat(result, &character, 1);
    }
    
    return result;
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
    char ftp[] = "ftp";
    char yes[] = "yes"; 
    char no[] = "no"; 
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
  
    n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL,
        ( struct sockaddr *) &cliaddr, &len);  

    int is_valid = strcmp("ftp", &buffer);
    char *response = "GOOD";
    /*
    if (is_valid != 0) {
        response = no;
    } else {
        response = yes;
    }
    */
    char *total_frag, *frag_no, *size, *filename, *frag;
    parsePacket(&buffer, &total_frag, &frag_no,
        &size, &filename, &frag);

    FILE *stream = fopen("output.txt", "w");
    if (!stream) {
        printf("File cannot be created.");
        return -1;
    }
    
    printf("%s\n", frag);
    int dataSize = strtonum(size, 0, 1000, NULL);
    char *data = bytesToString(frag, dataSize);

    fprintf(stream, data);

    //printf("%s\n", total_frag);
    //printf("%s\n", frag_no);
    //printf("%s\n", size);
    //printf("%s\n", filename);

    sendto(sockfd, (const char *)response, strlen(response),  
        NULL, (const struct sockaddr *) &cliaddr, len);

    return 0; 
}