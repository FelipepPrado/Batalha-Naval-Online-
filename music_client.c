// cliente.c
#include <stdio.h>
#include <winsock2.h>
#include <stdbool.h>
#include <locale.h>
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")

#define TAM 10
#define NUM_NAVIOS 9
#define SERVER_IP "127.0.0.1" // Endereço IP do servidor do pc que abrir o server
#define PORT 8080
#define TAMANHO_BUFFER 100

//Menu e jogar novamente
int rodar_jogo();
void menu();
void anim_navio();
void clearScreen();
int rept();

// Começa a música e o som do mar
void comecar_musica();
void comecar_mar();

// Para a música ou o mar
void parar_som();

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
bool receber_ataque(char mat_defesa[][TAM], int x, int y);

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    //Variaveis para receber a coordenada de ataque do Jogador 2
    int colbuffer, linbuffer;
    //Matrizes de Ataque e Defesa do Jogador 1
    char mat_def[TAM][TAM], mat_atk[TAM][TAM];
    //Variaveis para mandar a coordenada de ataque do Jogador 1
    int x, y;
    char col;
    //Variavel de controle para inicio do jogo assim que os dois colocarem os navios
    int jogo_ativo;
    int hitbuffer;
    int count_vt, count_dt;
    int ret, jogar_de_novo;
    //Variaveis para verificar se está ocorrendo a comunicação
    int verificar1, verificar2;
    setlocale(LC_ALL, "Portuguese");
    //Variaveis relacionadas aos nicknames dos jogadores
    char nick[TAMANHO_BUFFER];
    char nick_jogador2[TAMANHO_BUFFER];
    int valread;
    
    while(1){

        jogo_ativo = 1;
        
        if((ret=rodar_jogo())==0){
            return 0;
        }

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
            closesocket(clientSocket);
            WSACleanup();
            printf("Nenhum jogador encontrado\n");
            Sleep(2000);
            continue;
        }
        printf("Segundo jogador encontrado!!!\n");
        Sleep(1500);
        system("cls");
        
        //Solicita o nickname para o jogador
        printf("Insira seu nickname:\n");
        scanf("%s", nick);
        send(clientSocket, nick, strlen(nick), 0);
     	// Lê o nickname enviado pelo server e armazena na variavel nick_jogador2
    	valread = recv(clientSocket, nick_jogador2, TAMANHO_BUFFER - 1, 0);
        //Verifica se o nickname do jogador 2 foi armazenado com sucesso
	    system("cls");
        if (valread > 0){
        nick_jogador2[valread] = '\0';  // Garantir que a string tenha o terminador nulo
        printf("\nSeu adversário é %s\n\n", nick_jogador2);
	} else {
        printf("Jogador 2 não se conectou\n");
        printf("Retornando ao Menu\n");
        Sleep(3000);
        continue;
    }
    
	
        while(1) {
            count_vt = 0;
            count_dt = 0;

            //Inicializa as Matrizes
            inic_mat(mat_def);
            inic_mat(mat_atk);

            // Posiciona os navios do jogador
            print_mat(mat_def);
            printf("%s, posicione seus navios:\n", nick);
            posicionar_navios(mat_def);

            //Serve pro jogo começar apenas quando os dois terminarem de colocar os navios
            send(clientSocket, (char*)&jogo_ativo, sizeof(jogo_ativo), 0);
            
            //Para a música do menu
            parar_som();

            //Começa o som do mar
            comecar_mar();
            
            // Recebe dados do cliente
            while (jogo_ativo){
                //Limpa a tela para a vez do Jogador 2
                system("cls");
                printf("Vez do %s\n", nick_jogador2);

                //Verifica se o jogador 2 ainda está jogando
                verificar1 = recv(clientSocket,(char*)&linbuffer, sizeof(linbuffer), 0);
                verificar2 = recv(clientSocket,(char*)&colbuffer, sizeof(colbuffer), 0);
                if(verificar1 <= 0 || verificar2 <= 0){
                    printf("%s desconectou-se do jogo\n\n", nick_jogador2);
                    break;
                }

                //Verifica se o jogador 2 acertou e devolve um valor de acordo com isso
                if(receber_ataque(mat_def,linbuffer,colbuffer)){
                    hitbuffer = 1;
                    printf("SEU NAVIO FOI ACERTADO\n");
                    count_dt+=1;
                }
                else{
                    hitbuffer = 0;
                    printf("%s Errou!!!!!!!\n", nick_jogador2);
                }
                printf("%s (Defesa)\t\t%s (Ataque)\n", nick, nick);
                print2_mat(mat_def, mat_atk);

                //Envia para o jogador 2 se ele acertou ou não um navio
                send(clientSocket,(char*)&hitbuffer, sizeof(hitbuffer), 0);

                //Verifica se o jogador 1 perdeu
                if(count_dt == 18){
                    printf("%s voce perdeu!\n\n\n", nick);
                    parar_som();
                    break;
                }
                Sleep(1500);
                
                //Limpa a tela pro jogador 1 atacar
                system("cls");
                printf("%s (Defesa)\t\t%s (Ataque)\n", nick, nick);
                print2_mat(mat_def, mat_atk);
                

                // Jogador 1 ataca
                printf("\n%s, insira as coordenadas do ataque (ex: A 1): ", nick);
                scanf(" %c%d", &col, &y);
                x = col - 'A';
                //Realiza o som do disparo
                const char *disparo = "C:/Users/Patric/Documents/audiosbt/disparo.wav";
                PlaySound(disparo, NULL, SND_FILENAME | SND_ASYNC); // Som do disparo
                while(x < 0 || x >= TAM || y < 0 || y >= TAM){
                    system("cls");
                    printf("Coordenadas Invalidas!!\n");
                    printf("%s (Defesa)\t\t%s (Ataque)\n", nick, nick);
                    print2_mat(mat_def, mat_atk);

                    // Jogador 1 ataca
                    printf("\n%s, insira as coordenadas do ataque (ex: A 1): ", nick);
                    scanf(" %c%d", &col, &y);
                    x = col - 'A';
                    continue;
                }
                send(clientSocket, (char*)&x, sizeof(x), 0);
                send(clientSocket, (char*)&y, sizeof(y), 0);

                //Verifica se o jogador 2 ainda está jogando e Recebe se o jogador 1 acertou ou não um navio
                verificar1 = recv(clientSocket, (char*)&hitbuffer, sizeof(hitbuffer), 0);
                if(verificar1 <= 0){
                    printf("%s desconectou-se do jogo\n\n", nick_jogador2);
                    break;
                }
                system("cls");
                //Verifica o retorno do client se o jogador 1 acertou
                if(hitbuffer == 1){
                    mat_atk[x][y] = 'X'; // 'X' representa um navio atingido
                    printf("Acertou!!!\n");
                    count_vt+=1;
                }
                else{
                    if(mat_atk[x][y] == '~'){//Não tem o 'a' na condição pq a matriz ataque n possui esse 'a'
                        mat_atk[x][y] = 'O'; // 'O' representa um ataque na água
                        printf("Água!!!\n");
                    }
                }
                printf("%s (Defesa)\t\t%s (Ataque)\n", nick, nick);
                print2_mat(mat_def, mat_atk);
                Sleep(1500);
                comecar_mar(); // Volta o som do mar

                //Verifica se o Jogador 1 venceu
                if(count_vt == 18){
                    printf("%s venceu!\n\n", nick);
                    parar_som();
                    break;
                }
            }

            //Ele pergunta se quer jogar novamente, mas caso o jogador já não estiver em jogo, ele volta para o menu
            if(verificar1 > 0 && verificar2 > 0){
                if(rept()){
                    jogar_de_novo = 1;
                    send(clientSocket, (char*)&jogar_de_novo, sizeof(int), 0);
                    printf("Esperando resposta do %s\n", nick_jogador2);
                    jogar_de_novo = 0;//Server pra não continuar, caso o jogador 2 não querer jogor novamente
                    recv(clientSocket, (char*)&jogar_de_novo, sizeof(int), 0);
                    if(jogar_de_novo == 1){//Supondo que ele tenha respondido
                        printf("%s aceitou sua solicitação.\nRecomeçando o Jogo\n");
                        Sleep(3000);
                        system("cls");
                        continue;
                    }
                    else{
                        closesocket(clientSocket);
                        WSACleanup();
                        printf("%s não aceitou sua solicitação.\nRetornando ao Menu\n", nick_jogador2);
                        Sleep(3000);
                        break;
                    }
                }
                else{
                    // Fecha o socket do cliente
                    closesocket(clientSocket);
                    WSACleanup();
                    return 0;
                }
            }

            //Caso o jogador 2 não estiver no jogo ele já volta direto para o menu
            printf("Retornando ao Menu!");
            Sleep(3000);
            break;
        }
    }
}

//Começa a música do menu
void comecar_musica(){
    const char *musicamenu = "C:/Users/Patric/Documents/audiosbt/musicamenu.wav";
    PlaySound(musicamenu, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//Começa o som do mar
void comecar_mar(){
    const char *mar = "C:/Users/Patric/Documents/audiosbt/mar.wav";
    PlaySound(mar, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

//Para a música do menu ou o mar
void parar_som(){
    PlaySound(NULL, 0, 0);
}

//bota o jogo para rodar
int rodar_jogo(){
    int numero;
    system("cls");
    comecar_musica();
    anim_navio();
    system("cls");
    while (1) {
        menu();
        printf("Escolha uma opcao: ");
        scanf("%d", &numero);
        system("cls"); 
        
        switch (numero) {
            case 1:
                printf("Entrando no jogo...\n");
                Sleep(1500);
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
        printf("Deseja jogar novamente?\n\n1. sim\n2 .nao\n");
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

void clearScreen() {
    printf("\033[H\033[J");
}

void anim_navio() {
    const char* navioDireita[] = {
        "                          |    |    |",
        "                         )_)  )_)  )_)",
        "                        )___))___))___)",
        "                       )____)____)_____)",
        "                     _____|____|____|____",
        "   \\                 \\----------------/"
    };


    const char* onda1[] = {
        "~~~~~~~~~~~~~~~~~~\\     ~~~~~~      ~~~~~~~~~~~~~~",
        "  ~~~~ ~~~ ~~~~~~~~\\~~~~~~~~  ~~~~~~~~~~ ~~~ ~~~ ~~",
        "   ~~~~  ~~~~~~~~~  ~~~ ~~~  ~~~~  ~~~ ~~",
        "      ~~~~  ~~~~   ~~~~~~  ~~~~~~   ~~~"
    };

    const char* onda2[] = {
        "  ~~~~~~~~  ~~~~\\     ~~~~~~    ~~~~~~~~~~~",
        "~~~~  ~~~ ~~~~~~~~\\~~~~~~~~ ~~~~~~~ ~~~ ~~ ~~~~",
        "  ~~~ ~~~~~~~~~  ~~~~  ~~~  ~~~~  ~~~~ ~~",
        "~~~    ~~~~  ~~~~   ~~~~~~  ~~~~~~   ~~~"
    };

    int passos = 40;
    int direc = 1;
    int posic = 0;

     struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 200000000;
    int count=0;

    while (count<=20) {
        clearScreen();
        for (int j = 0; j < 6; j++) {
            for (int k = 0; k < posic; k++) {
                printf(" ");
            }
            printf("%s\n", navioDireita[j]);
        }

        const char** onda = (posic % 2 == 0) ? onda1 : onda2;
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < posic; k++) {
                printf(" ");
            }
            printf("%s\n", onda[j]);
        }
        nanosleep(&delay, NULL);
        posic += direc;
        count++;
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

void print_mat(char mat[][TAM]) {
    printf("  ");
    for (int j = 0; j < TAM; j++) {
        printf("%d ", j);
    }
    putchar('\n');
    
    for (int i = 0; i < TAM; i++) {
        printf("%c ", 'A' + i);
        for (int j = 0; j < TAM; j++) {
            switch (mat[i][j]) {
                case '~':  // Fundo azul
                case 'a':
                    printf("\033[44m \033[0m ");
                    break;
                case 'N':  // Fundo verde
                    printf("\033[42mN\033[0m ");
                    break;
                case 'O':  // Texto ciano
                    printf("\033[36mO\033[0m ");
                    break;
                case 'X':  // Texto vermelho
                    printf("\033[31mX\033[0m ");
                    break;
                default:   // Caso padrão
                    printf("%c ", mat[i][j]);
            }
        }
        putchar('\n');
    }
}

void print_coluna() {
    // Imprime o cabeçalho com números das colunas
    printf("  ");
    for (int j = 0; j < TAM; j++) {
        printf("%d ", j);
    }
}

void print_linha(char mat[][TAM], int row) {
    // Imprime uma linha da matriz com a formatação de cores
    printf("%c ", 'A' + row);
    for (int j = 0; j < TAM; j++) {
        switch (mat[row][j]) {
            case '~':  // Fundo azul
            case 'a':
                printf("\033[44m \033[0m ");
                break;
            case 'N':  // Fundo verde
                printf("\033[42mN\033[0m ");
                break;
            case 'O':  // Texto ciano
                printf("\033[36mO\033[0m ");
                break;
            case 'X':  // Texto vermelho
                printf("\033[31mX\033[0m ");
                break;
            default:   // Caso padrão
                printf("%c ", mat[row][j]);
        }
    }
}

void print2_mat(char mat1[][TAM], char mat2[][TAM]) {
    // Imprime os cabeçalhos das colunas para ambas as matrizes
    print_coluna();
    printf("\t");
    print_coluna();
    putchar('\n');

    // Imprime as linhas de ambas as matrizes lado a lado
    for (int i = 0; i < TAM; i++) {
        print_linha(mat1, i);
        printf("\t");
        print_linha(mat2, i);
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

    if((sent == 'V' || sent == 'v') && (x-1) >= 0 && (x+1) < TAM && mat[x-1][y]=='~' && mat[x+1][y]=='~'){
        mat[x+1][y]='N';
        mat[x-1][y]='N';
        area_nav(mat,x-1,y);
        area_nav(mat,x+1,y);
        system("cls");
        printf("Tabuleiro atual:\n");
        print_mat(mat); 
        return 1; // Sucesso
    }

    if((sent == 'H' || sent == 'h') && (y-1) >= 0 && (y+1) < TAM && mat[x][y-1]=='~' && mat[x][y+1]=='~'){
        mat[x][y+1]='N';
        mat[x][y-1]='N';
        area_nav(mat,x,y-1);
        area_nav(mat,x,y+1);
        system("cls");
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
            system("cls");
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
            system("cls");
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
                system("cls");
                printf("Tabuleiro atual:\n");
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

bool receber_ataque(char mat_defesa[][TAM], int x, int y){
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
