#include <stdlib.h>
#include <stdio.h>
#define C 2
const int val = 16;
char arr[] = "foo";
void foo(int arg){
	    char *str = (char *) malloc (C*val);
		    char *ptr = arr;
			    free(str);
}

int main() {
	    foo(15);
}

