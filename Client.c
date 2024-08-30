// servidor.c
#include <stdio.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int colbuffer[2];
    char linbuffer[2];
    int bytesReceived;

    // Inicializa o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicialização do Winsock.\n");
        return 1;
    }

    // Cria o socket do servidor
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        printf("Erro ao criar o socket do servidor.\n");
        WSACleanup();
        return 1;
    }

    // Configura o endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Associa o socket a um endereço e porta
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Erro ao associar o socket.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Escuta por conexões
    if (listen(serverSocket, 3) == SOCKET_ERROR) {
        printf("Erro ao escutar.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Aguardando conexões na porta %d...\n", PORT);

    // Aceita uma conexão
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        printf("Erro ao aceitar conexão.\n");
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    printf("Cliente conectado.\n");

    // Recebe dados do cliente
    while ((bytesReceived = recv(clientSocket,(char*)&linbuffer[0], sizeof(char), 0)) > 0 && (bytesReceived =  recv(clientSocket,(char*)&colbuffer[0], sizeof(int), 0))) {
        printf("Recebido: %c e %d\n", linbuffer[0],colbuffer[0]);
        scanf("%c %d",&linbuffer[1], &colbuffer[1]);
        // Envia uma resposta ao cliente
        send(clientSocket, (char*)&linbuffer[1], sizeof(char), 0);
        send(clientSocket, (char*)&colbuffer[1], sizeof(int), 0);
    }

    if (bytesReceived == SOCKET_ERROR) {
        printf("Erro ao receber dados.\n");
    }

    // Fecha o socket do cliente e do servidor
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
