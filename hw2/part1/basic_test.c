#include<stdio.h>

unsigned short count_above(char separator,long limit);

int main() {
	printf("%hi\n",count_above(',', 10));
	printf("%hi\n",count_above(',', 1000));
	printf("%hi\n",count_above(',', 10000));
	return 0;
}