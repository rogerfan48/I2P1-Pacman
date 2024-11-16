#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int generateRandomNumber(int a, int b) {
	return rand() % (b - a + 1) + a;
}

int main() {
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
    printf("%d\n", generateRandomNumber(0,3));
}