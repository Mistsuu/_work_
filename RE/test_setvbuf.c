#include <stdio.h>

int main() {
    setvbuf(stdin, 0, 2, 0);
    setvbuf(stdout, 0, 2, 0);

    printf("%d\n",_IOFBF);
    printf("%d\n",_IOLBF);
    printf("%d\n",_IONBF);

    char str[10000];
    scanf("%s", str);
    printf("%s", str);

    while (1) {}
}