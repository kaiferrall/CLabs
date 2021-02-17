#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 

//client
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

//server
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