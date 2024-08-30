// cliente.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET sock;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE] = {0};
    const char *message = "Olá do cliente!";

    // Inicializar o Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Falha na inicialização do Winsock. Código do erro: %d\n", WSAGetLastError());
        return 1;
    }

    // Criar o socket
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) {
        printf("Não foi possível criar o socket. Código do erro: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Configurar a estrutura do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("ENDERECO_IP_DO_SERVIDOR"); // Substitua pelo IP real do servidor
    serverAddr.sin_port = htons(PORT);

    // Conectar ao servidor
    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("Conexão falhou. Código do erro: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Enviar mensagem ao servidor
    send(sock, message, strlen(message), 0);

    // Receber a resposta do servidor
    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Mensagem recebida do servidor: %s\n", buffer);

    // Fechar o socket
    closesocket(sock);
    WSACleanup();

    return 0;
}
