#include<stdio.h>
#include<stdlib.h>
#include <string.h>

int main() {
unsigned char * ch;
ch = (unsigned char *)malloc(16);
memset(ch, '0', 16);
printf("%s", ch);
}
