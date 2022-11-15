#include <stdio.h>
#include <string.h>

int main()
{
	char buf1[50], buf2[50];
	int res;
	printf("Please input your name: ");
	res = fgets(buf1, 10, stdin);
	printf("res = %d\n", res);
	printf("Please input your passwd: ");
	res = fgets(buf2, 10, stdin);
	printf("res = %d\n", res);
	printf("buf1's len = %d buf2's len = %d\n", strlen(buf1), strlen(buf2));	
	printf("%s\n%s\n", buf1, buf2);
	return 0;
}
