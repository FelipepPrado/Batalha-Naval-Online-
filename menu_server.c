// servidor.c
#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <stdbool.h>
#include <locale.h>

#pragma comment(lib, "ws2_32.lib")

#define TAM 10
#define NUM_NAVIOS 9
#define PORT 8080

int rodar_jogo();
void menu();
int rept();

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
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    //Matrizes para mandar e receber posições
    int colbuffer;
    int linbuffer;
    int bytesReceived;
    char mat_def[TAM][TAM], mat_atk[TAM][TAM];//Matrizes de Ataque e defesa
    int x, y;
    char col;
    int jogo_ativo;
    int hitbuffer, count_vt,count_dt;
    setlocale(LC_ALL, "Portuguese");
    while(1){
        count_vt = 0;
        count_dt = 0;
        jogo_ativo = 1;
        int ret, jogar_de_novo;
        if((ret=rodar_jogo())==0){
            return 0;
        }
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

        printf("Aguardando um segundo jogador na porta %d...\n", PORT);

        // Aceita uma conexão
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            printf("Erro ao aceitar conexão.\n");
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        printf("Segundo jogador encontrado!!!\n");
        while(1){
            //Inicializa as Matrizes
            inic_mat(mat_def);
            inic_mat(mat_atk);

            // Posiciona os navios do jogador
            print_mat(mat_def);
            printf("Jogador 1, posicione seus navios:\n");
            posicionar_navios(mat_def);
            
            recv(clientSocket, (char*)&jogo_ativo, sizeof(jogo_ativo),0);

            // Recebe dados do cliente
            while (jogo_ativo) {
                system("cls");
                printf("Jogador 1 (Defesa)\t\tJogador 1 (Ataque)\n");
                print2_mat(mat_def, mat_atk);

                // Jogador 1 ataca
                printf("\nJogador 1, insira as coordenadas do ataque (ex: A 1): ");
                scanf(" %c%d", &col, &y);
                x = col - 'A';
                if(x < 0 || x >= TAM || y < 0 || y >= TAM){//MELHORAR ESSA PARTE
                    printf("Coordenadas Invalidas!!\n");
                    continue;
                }
                send(clientSocket, (char*)&x, sizeof(x), 0);
                send(clientSocket, (char*)&y, sizeof(y), 0);

                recv(clientSocket, (char*)&hitbuffer, sizeof(hitbuffer), 0);
                
                if(hitbuffer == 1){
                    mat_atk[x][y] = 'X'; // 'X' representa um navio atingido
                    printf("Acertou!\n");
                    count_vt+=1;
                }
                else{
                    if(mat_atk[x][y] == '~'){//Não tem o 'a' na condição pq a matriz ataque n possui esse 'a'
                        mat_atk[x][y] = 'O'; // 'O' representa um ataque na água
                        printf("Água!\n");
                    }
                }

                if(count_vt == 18){
                    printf("Jogador 1 venceu!\n");
                    break;
                }
                //Limpa a tela para a vez do Jogador 2
                system("cls");
                printf("Vez do Jogador 2\n");
                recv(clientSocket,(char*)&linbuffer, sizeof(linbuffer), 0);
                recv(clientSocket,(char*)&colbuffer, sizeof(colbuffer), 0);
                if(realizar_ataque(mat_def,linbuffer, colbuffer)){
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

                send(clientSocket,(char*)&hitbuffer, sizeof(int), 0);

                if(count_dt == 18){
                    printf("Jogador 1 você perdeu!\n");
                    break;
                }
            }
            if(rept()){
                jogar_de_novo = 0;
                recv(clientSocket, (char*)&jogar_de_novo, sizeof(int), 0);
                if(jogar_de_novo == 1){//Supondo que ele tenha respondido
                    jogar_de_novo = 1;
                    send(clientSocket, (char*)&jogar_de_novo, sizeof(int), 0);
                    continue;
                }
                else{
                    closesocket(clientSocket);
                    WSACleanup();
                    printf("Jogador 2 não aceitou sua solicitação.\nRetornando ao Menu\n");
                    sleep(2);
                    break;
                }
            }
            else{
                // Fecha o socket do cliente
                closesocket(clientSocket);
                WSACleanup();
                printf("Retornando ao Menu\n");
                sleep(2);
                break;
            }
        }
    }
}

//bota o jogo para rodar
int rodar_jogo(){
    int numero;
    system("cls");
    while (1) {
        menu();
        printf("Escolha uma opcao: ");
        scanf("%d", &numero);
        system("cls"); 
        
        switch (numero) {
            case 1:
                printf("Entrando no jogo...\n");
                return 1;
                
            case 2:
                printf("Saindo...\n");
                return 0;
            default:
                printf("Opcao invalida. Tente novamente.\n");
        }
    }
}

void menu() {
    // Código ANSI para a cor azul
    printf("\033[1;34m");

    // Imprimir o texto em formato ASCII
    printf("* d8888b.  .d8b.  d888888b  .d8b.  db      db   db  .d8b.       d8b   db  .d8b.  db    db  .d8b.  db *\n");
    printf("* 88  `8D d8' `8b `~~88~~' d8' `8b 88      88   88 d8' `8b      888o  88 d8' `8b 88    88 d8' `8b 88 *\n");
    printf("* 88oooY' 88ooo88    88    88ooo88 88      88ooo88 88ooo88      88V8o 88 88ooo88 Y8    8P 88ooo88 88 *\n");
    printf("* 88~~~b. 88~~~88    88    88~~~88 88      88~~~88 88~~~88      88 V8o88 88~~~88 `8b  d8' 88~~~88 88 *\n");
    printf("* 88   8D 88   88    88    88   88 88booo. 88   88 88   88      88  V888 88   88  `8bd8'  88   88 88booo. *\n");
    printf("* Y8888P' YP   YP    YP    YP   YP Y88888P YP   YP YP   YP      VP   V8P YP   YP    YP    YP   YP Y88888P *\n");
    // Resetar a cor para o padrão
    printf("\033[0m");


    // Espaço antes do menu para clareza
    printf("\n");
   

    // Menu de opções
    printf("\t\t\t\t\t1. Jogar\n");
    printf("\t\t\t\t\t2. Sair\n");

    // Linha divisória
    printf("\n\t\t\t\t\t===========================\n");
}

int rept(){
    int jogar_de_novo;
    while (1){
        printf("Deseja jogar novamente\n\n1. sim\n2 .nao?");
        scanf("%d", &jogar_de_novo);
        system("cls");

        switch (jogar_de_novo){
                case 1:
                    return 1;                
                case 2:
                    return 0;
                default:
                    printf("Opção invalida. Tente novamente.\n");
        }
    }
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
    if(sent != 'V' && sent != 'v' && sent != 'H' && sent != 'h'){
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

    if(sent == 'V' || sent == 'v' && (x-1) >= 0 && (x+1) < TAM && mat[x-1][y]=='~' && mat[x+1][y]=='~'){
        mat[x+1][y]='N';
        mat[x-1][y]='N';
        area_nav(mat,x-1,y);
        area_nav(mat,x+1,y);
        printf("Tabuleiro atual:\n");
        print_mat(mat); 
        return 1; // Sucesso
    }

    if(sent == 'H' || sent == 'h' && (y-1) >= 0 && (y+1) < TAM && mat[x][y-1]=='~' && mat[x][y+1]=='~'){
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

    if(sent != 'V' && sent != 'v' && sent != 'H' && sent != 'h'){
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
    else if(mat_defesa[x][y] == '~' || mat_defesa[x][y] == 'a'){
        mat_defesa[x][y] = 'O'; // 'O' representa um ataque na água
        return false;
    }
    return false;
}