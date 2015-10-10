#include<stdio.h>
#include<stdlib.h>
#include<time.h>
int main() {
	int i;
	for(i = 0; i < 10; i++) {
		sleep(1);
		printf("%d ", i);
		
	}
	return 0;
}
