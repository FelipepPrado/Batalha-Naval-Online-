// cliente.c
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // Endereço IP do servidor
#define PORT 8080

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    int colbuffer[2];
    char linbuffer[2];
    int bytesReceived;

    // Inicializa o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicialização do Winsock.\n");
        return 1;
    }

    // Cria o socket do cliente
    clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro ao criar o socket do cliente.\n");
        WSACleanup();
        return 1;
    }

    // Configura o endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    // Conecta ao servidor
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erro ao conectar ao servidor.\n");
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    scanf("%c %d",&linbuffer[0], &colbuffer[0]);
    // Envia dados ao servidor
    send(clientSocket, (char*)&linbuffer[0], sizeof(char), 0);
    send(clientSocket, (char*)&colbuffer[0], sizeof(int), 0);

    // Recebe resposta do servidor
    bytesReceived = recv(clientSocket, (char*)&linbuffer[1], sizeof(char), 0);
    recv(clientSocket, (char*)&colbuffer[1], sizeof(int), 0);
    if (bytesReceived > 0) {
        printf("Resposta do servidor: %c e %d\n", linbuffer[1],colbuffer[1]);
    }

    // Fecha o socket do cliente
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
