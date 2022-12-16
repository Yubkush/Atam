#include<stdio.h>
#include<assert.h>

unsigned short count_above(char separator,long limit);

int main() {
	short res = count_above('_', 94);
	if(res != 0)	{
		printf("Test 33 failed:\n");
		printf("	Test 33 output: %hi\n",res);
		printf("	Test 33 expected: 0\n");
	}
	else printf("Test 33 passed\n");
	return 0;
}