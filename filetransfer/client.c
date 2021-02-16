// Client side implementation of UDP client-server model 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define PORT     8080 
#define MAXLINE 1024 
#define FRAG_SIZE 1024

struct packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* filename;
    char filedata[FRAG_SIZE];
};
/*
char *createPacket(char *dest, unsigned int total_frag, unsigned int frag_no,
                     unsigned int size) {

}
*/
int main() { 
    int sockfd; 
    char buffer[MAXLINE]; 
    char *ftp = "ftp"; 
    struct sockaddr_in     servaddr; 
  
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n, len; 
    
    sendto(sockfd, (const char *)ftp, strlen(ftp), 
        NULL, (const struct sockaddr *) &servaddr,  
            sizeof(servaddr)); 
          
    n = recvfrom(sockfd, (char *)buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len); 
    buffer[n] = '\0'; 
    if (strcmp("yes", buffer) != 0) {
        printf("Failed.");
        return -1;
    } 

    FILE *stream = fopen("data.txt", "r");
    if (!stream) {
        printf("Does not exist");
        fclose(stream);
        return -1;
    } 
    fseek(stream, 0L, SEEK_END); 
    long int streamEnd = ftell(stream);
    
    int d;
    unsigned int frag_no = 0;
    unsigned int total_frag = (int)(streamEnd/FRAG_SIZE) + 1;
    unsigned int size = 1000;
    char *filename = "data.txt";
    char frag[FRAG_SIZE];
    
    d = fread(&frag, sizeof(char), FRAG_SIZE, stream);
    char cPacket[];
    sprintf(cPacket, "%d:%d:%d:%s:", total_frag, frag_no, size, filename);
    printf("%s", cPacket);

    close(sockfd); 
    return 0; 
} 