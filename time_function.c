#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int seconds = 30;
    while(seconds > 0) {
        system("cls"); 
        printf("Tempo restante: %d segundos\n", seconds);
        sleep(1);
        seconds--;
    }

    system("cls");  
    printf("Tempo esgotado!\n");

    return 0;
}
