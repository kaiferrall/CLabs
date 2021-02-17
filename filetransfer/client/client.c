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
#define FRAG_SIZE 1000

static const char *FILENAME = "data.txt";

struct Packet {
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* filename;
    char filedata[FRAG_SIZE];
};

void printPacket(struct Packet p) {
    printf("\n-- Packet --\n");
    printf("total_frag = %d | frag_no = %d | size = %d | filename = %s\n", 
        p.total_frag, p.frag_no, p.size, p.filename);
    printf("data: %s", p.filedata);
}

char *stringToBinary(char* s) {
    if(s == NULL) return 0; 
    size_t len = FRAG_SIZE/8;
    char *binary = malloc(FRAG_SIZE);

    for(size_t i = 0; i < len; ++i) {
        char ch = s[i];
        for(int j = 7; j >= 0; --j){
            if(ch & (1 << j)) {
                strcat(binary,"1");
            } else {
                strcat(binary,"0");
            }
        }
    }
    return binary;
}

int main() { 
    int sockfd; 
    char buffer[MAXLINE]; 
    char ftp[] = "ftp"; 
    struct sockaddr_in servaddr; 
    // Creating socket file descriptor 
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(PORT); 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
      
    int n, len; 
    /*
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
    */ 
    FILE *stream = fopen(FILENAME, "rb");
    if (!stream) {
        printf("File does not exist");
        fclose(stream);
        return -1;
    } 
    //get total size of file
    fseek(stream, 0L, SEEK_END); 
    long int streamSize = ftell(stream);
    fseek(stream, 0L, 0L);
    //points to current frag location in file    
    long int streamPtr = ftell(stream);
    //initial packet
    struct Packet cPacket; 
    cPacket.filename = FILENAME;
    cPacket.total_frag = (int)(((streamSize)*8)/1000) + 1;
    cPacket.frag_no = 1;

    while (cPacket.frag_no <= cPacket.total_frag) {
        //check if frag excedes eof
        fread(cPacket.filedata, 1, FRAG_SIZE/8, stream);
        streamPtr = ftell(stream);
        //convert file data to byte string
        char *binData = stringToBinary(cPacket.filedata);
        cPacket.size = strlen(binData);
        //serialize current packet
        char *serializedPacket;
        asprintf(&serializedPacket, "%d:%d:%d:%s:%s", cPacket.total_frag, 
            cPacket.frag_no, cPacket.size, cPacket.filename, binData);
        
        //printPacket(cPacket);
        //send packet
        sendto(sockfd, (const char *)serializedPacket, strlen(serializedPacket), 
            0, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        cPacket.frag_no += 1;
        memset(cPacket.filedata, 0, sizeof(cPacket.filedata));
    }

    close(sockfd); 
    return 0; 
} 