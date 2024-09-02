// cliente.c
#include <stdio.h>
#include <winsock2.h>
#include <stdbool.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib")

#define TAM 10
#define NUM_NAVIOS 9
#define SERVER_IP "127.0.0.1" // Endereço IP do servidor do pc que abrir o server
#define PORT 8080

// Inicia a tabela da batalha naval
void inic_mat(char mat[][TAM]);

void area_nav(char mat[][TAM],int x,int y);

void print_mat(char mat[][TAM]);

// Printar duas matrizes lado a lado com rótulos
void print2_mat(char mat1[][TAM], char mat2[][TAM]);

// As 3 funcoes retornam 1 se bem-sucedida, 0 se fracassada
bool posicionar_navio_tam3(int i, char mat[][TAM]);

bool posicionar_navio_tam2(int i, char mat[][TAM]);

bool posicionar_navio_tam1(int i, char mat[][TAM]);

// Posiciona navios no tabuleiro
void posicionar_navios(char mat[][TAM]);

// Realiza um ataque na matriz do adversário
bool realizar_ataque(char mat_defesa[][TAM], int x, int y);

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    //Variáveis para atacar[0](posição 0 é a que manda) e ser atacado[1](posição 1 é a)
    int colbuffer;
    int linbuffer;
    int bytesReceived;
    char mat_def[TAM][TAM], mat_atk[TAM][TAM];//Matrizes de Ataque e defesa
    int x, y;
    char col;
    int jogo_ativo = 1;
    int hitbuffer, count_vt, count_dt;
    count_vt = 0;
    count_dt = 0;

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
    printf("Segundo jogador encontrado!!!\n");

    //Inicializa as Matrizes
    inic_mat(mat_def);
    inic_mat(mat_atk);

    // Posiciona os navios do jogador
    print_mat(mat_def);
    printf("Jogador 1, posicione seus navios:\n");
    posicionar_navios(mat_def);

    
    send(clientSocket, (char*)&jogo_ativo, sizeof(jogo_ativo), 0);
    while(jogo_ativo){
        //Limpa a tela para a vez do Jogador 2
        system("cls");
        printf("Vez do Jogador 2\n");

        recv(clientSocket,(char*)&linbuffer, sizeof(linbuffer), 0);
        recv(clientSocket,(char*)&colbuffer, sizeof(colbuffer), 0);

        if(realizar_ataque(mat_def,linbuffer,colbuffer)){
            hitbuffer = 1;
            printf("SEU NAVIO FOI ACERTADO\n");
            count_dt+=1;
        }
        else{
            hitbuffer = 0;
            printf("Jogador 2 Errou!!!!!!!\n");
        }
        printf("Jogador 1 (Defesa)\t\tJogador 1 (Ataque)\n");
        print2_mat(mat_def, mat_atk);

        send(clientSocket,(char*)&hitbuffer, sizeof(hitbuffer), 0);

        if(count_dt == 18){
            printf("Jogador 1 você perdeu!\n");
            jogo_ativo = 0;
            break;
        }

        system("cls");
        printf("Jogador 1 (Defesa)\t\tJogador 1 (Ataque)\n");
        print2_mat(mat_def, mat_atk);

        // Jogador 1 ataca
        printf("\nJogador 1, insira as coordenadas do ataque (ex: A 1): ");
        scanf(" %c%d", &col, &y);
        x = col - 'A';
        if(x < 0 && x >= TAM && y < 0 && y >= TAM){
            printf("Coordenadas Invalidas!!\n");
            continue;
        }
        send(clientSocket, (char*)&x, sizeof(x), 0);
        send(clientSocket, (char*)&y, sizeof(y), 0);

        recv(clientSocket, (char*)&hitbuffer, sizeof(int), 0);
        
        if(hitbuffer == 1){
            mat_atk[x][y] = 'X'; // 'X' representa um navio atingido
            printf("Acertou!\n");
            count_vt+=1;
        }
        else{
            mat_atk[x][y] = 'O'; // 'O' representa um ataque na água
            printf("Água!\n");
        }

        if(count_vt == 18){
            printf("Jogador 1 venceu!\n");
            jogo_ativo = 0;
            break;
        }
    }

    // Fecha o socket do cliente
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}

void inic_mat(char mat[][TAM]){
    int i, j;
    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            mat[i][j] = '~'; // Inicialmente apenas água
        }
    }
}

void area_nav(char mat[][TAM],int x,int y){
	int i,j;
	for(i=x-1;i<=x+1;i++){
		for(j=y-1;j<=y+1;j++){
			if(mat[i][j]=='~' && i>=0 && j>=0 && i<TAM && j<TAM){
				mat[i][j]='a';
			}
		}
	}
}

void print_mat(char mat[][TAM]){
    int i, j;
    printf("  "); 
    for(j = 0; j < TAM; j++){
        printf("%d ", j);
    }
    putchar('\n');
    
    for(i = 0; i < TAM; i++){
        printf("%c ", 'A' + i);
        for(j = 0; j < TAM; j++){
            if (mat[i][j] == '~' || mat[i][j] == 'a') {
                printf("\033[44m \033[0m ");  // Fundo azul
            } else if (mat[i][j] == 'N') {
                printf("\033[42mN\033[0m ");  // Fundo verde
            } else if (mat[i][j] == 'O') {
                printf("\033[36mO\033[0m ");  // Texto ciano
            } else if (mat[i][j] == 'X') {
                printf("\033[31mX\033[0m ");  // Texto vermelho
            } else {
                printf("%c ", mat[i][j]);  // Padrão
            }
        }
        putchar('\n');
    }
}

void print2_mat(char mat1[][TAM], char mat2[][TAM]){
    int i, j;
    
    printf("  ");
    for(j = 0; j < TAM; j++){
        printf("%d ", j);
    }
    printf("\t");
    printf("  ");
    for(j = 0; j < TAM; j++){
        printf("%d ", j);
    }
    putchar('\n');

    for(i = 0; i < TAM; i++){
        printf("%c ", 'A' + i);
        for(j = 0; j < TAM; j++){
            if (mat1[i][j] == '~' || mat1[i][j] == 'a') {
                printf("\033[44m \033[0m ");
            } else if (mat1[i][j] == 'N') {
                printf("\033[42mN\033[0m ");
            } else if (mat1[i][j] == 'O') {
                printf("\033[36mO\033[0m ");
            } else if (mat1[i][j] == 'X') {
                printf("\033[31mX\033[0m ");
            } else {
                printf("%c ", mat1[i][j]);
            }
        }

        printf("\t");

        printf("%c ", 'A' + i);
        for(j = 0; j < TAM; j++){
            if (mat2[i][j] == '~' || mat2[i][j] == 'a') {
                printf("\033[44m \033[0m ");
            } else if (mat2[i][j] == 'N') {
                printf("\033[42mN\033[0m ");
            } else if (mat2[i][j] == 'O') {
                printf("\033[36mO\033[0m ");
            } else if (mat2[i][j] == 'X') {
                printf("\033[31mX\033[0m ");
            } else {
                printf("%c ", mat2[i][j]);
            }
        }
        putchar('\n');
    }
}

bool posicionar_navio_tam3(int i, char mat[][TAM]){
    char linha,sent;
    int x,y;

    printf("Defina a direção do navio de tamanho 3 em Vertica(V) ou Horizontal(H): ");
    scanf(" %c", &sent);
    if(sent != 'V' && sent != 'H'){
        printf("Direção invalida\n");
        return 0;
    }
    
    printf("Posicione o centro do %d navio de tamanho 3 (ex: A 1): ", i + 1);
    scanf(" %c %d", &linha, &y);
    fflush(stdin);
    
    x = linha - 'A'; // Converte a letra da linha para índice numérico

    if(x >= 0 && x < TAM && y >= 0 && y < TAM && mat[x][y] == '~'){
        mat[x][y]='N';
    }
    else {
        printf("Posição inválida ou ocupada. Tente novamente.\n");
    	return 0; // Indica o fracasso na operacao
    }

    if(sent == 'V' && (x-1) >= 0 && (x+1) < TAM && mat[x-1][y]=='~' && mat[x+1][y]=='~'){
        mat[x+1][y]='N';
        mat[x-1][y]='N';
        area_nav(mat,x-1,y);
        area_nav(mat,x+1,y);
        printf("Tabuleiro atual:\n");
        print_mat(mat); 
        return 1; // Sucesso
    }

    if(sent == 'H' && (y-1) >= 0 && (y+1) < TAM && mat[x][y-1]=='~' && mat[x][y+1]=='~'){
        mat[x][y+1]='N';
        mat[x][y-1]='N';
        area_nav(mat,x,y-1);
        area_nav(mat,x,y+1);
        printf("Tabuleiro atual:\n");
        print_mat(mat);
        return 1; // Sucesso
    }
    
    printf("Posição inválida ou ocupada. Tente novamente\n");
    mat[x][y]='~';
    return 0;      
}

bool posicionar_navio_tam2(int i,char mat[][TAM]){
    char linha,sent;
    int x,y;
    
    printf("Defina a direção do navio de tamanho 2 em Vertica(V) ou Horizontal(H): ");
    scanf(" %c", &sent);

    if(sent != 'V' && sent != 'H'){
        printf("Direção invalida\n");
        return 0;
    }

    if(sent == 'V' || sent == 'v'){
        printf("Posicione o extremo de cima do navio %d de tamanho 2 (ex: A 1): ", i + 1);
        scanf(" %c %d", &linha, &y);
        fflush(stdin);
        x = linha - 'A'; // Converte a letra da coluna para índice numérico

        if(x >= 0 && x+1 < TAM && y >= 0 && y < TAM && mat[x][y] == '~' && mat[x+1][y] == '~'){
            mat[x][y]='N';
            mat[x+1][y]='N';
            area_nav(mat,x,y);
            area_nav(mat,x+1,y);
            printf("Tabuleiro atual:\n");
            print_mat(mat);
            return 1; // Sucesso    
        }
    }
    if(sent == 'H' || sent == 'h'){
        printf("Posicione o extremo da esquerda do navio %d de tamanho 2 (ex: A 1): ", i + 1);
        scanf(" %c %d", &linha, &y);
        fflush(stdin);
        x = linha - 'A'; // Converte a letra da coluna para índice numérico

        if(x >= 0 && x < TAM &&  y >= 0 && y+1 < TAM && mat[x][y] == '~' && mat[x][y+1] == '~'){
            mat[x][y]='N';
            mat[x][y+1]='N';
            area_nav(mat,x,y);
            area_nav(mat,x,y+1);
            printf("Tabuleiro atual:\n");
            print_mat(mat);
            return 1; // Sucesso    
        }
    }
    printf("Posição Invalida ou ocupada. Tente novamente\n");
    return 0;

}

bool posicionar_navio_tam1(int i,char mat[][TAM]){
    char linha;
    int x,y;

    printf("Posicione o navio %d de tamanho 1 (ex: A 1): ", i + 1);
            scanf(" %c %d", &linha, &y);
            x = linha - 'A'; // Converte a letra da coluna para índice numérico

            if(x >= 0 && x < TAM && y >= 0 && y < TAM && mat[x][y] == '~'){
            	mat[x][y]='N';
            	area_nav(mat,x,y);
            	print_mat(mat);	
                return 1;
            }
            printf("Posição inválida ou ocupada. Tente novamente.\n");
            return 0;
}

void posicionar_navios(char mat[][TAM]){
    int i;
    
    for(i = 0; i < NUM_NAVIOS; i++){
        //Primeiro o jogador coloca os navios de tamanho 3
		if(i<3){
            if(posicionar_navio_tam3(i,mat) == 0) 
                --i;
            continue;
        }
        
        // Depois o jogador coloca os navios de tamanho 2
        if(i<6){
            if(posicionar_navio_tam2(i,mat) == 0)
                --i;
            continue;
        }
        // Por ultimo, tamanho 1
		else{
			if(posicionar_navio_tam1(i,mat) == 0)
                --i;
            continue;
		}
    }
}

bool realizar_ataque(char mat_defesa[][TAM], int x, int y){
    if(mat_defesa[x][y] == 'N'){
        mat_defesa[x][y] = 'X'; // 'X' representa um navio atingido
        return true;
    }
    else{
        mat_defesa[x][y] = 'O'; // 'O' representa um ataque na água
        return false;
    }
}
