#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAM 10
#define NUM_NAVIOS 9

// Inicia a tabela da batalha naval
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


// Printar duas matrizes lado a lado com rótulos
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

// Posiciona navios no tabuleiro
void posicionar_navios(char mat[][TAM]){
    int i, x, y;
    char col;
    char sent; //sentido
    
    for(i = 0; i < NUM_NAVIOS; i++){
        //Primeiro o jogador coloca os navios de tamanho 3
		if(i<3){
            printf("Posicione o centro do navio %d de tamanho 3 (ex: A 1): ", i + 1);
            scanf(" %c %d", &col, &y);
            fflush(stdin);
            putchar('\n');
            x = col - 'A'; // Converte a letra da coluna para índice numérico

            if(x >= 0 && x < TAM && y >= 0 && y < TAM && mat[x][y] == '~'){
            	mat[x][y]='N';
            }
            else {
            	printf("Posição inválida ou ocupada. Tente novamente.\n");
            	i--; // Repete a iteração
            	continue;
            }
            printf("Defina a direção do navio em Vertica(V) ou Horizontal(H): ");
            scanf(" %c", &sent);
            fflush(stdin);
            if(sent == 'V' && (x-1) >= 0 && (x+1) < TAM && mat[x-1][y]=='~' && mat[x+1][y]=='~'){
                mat[x+1][y]='N';
                mat[x-1][y]='N';
                area_nav(mat,x-1,y);
                area_nav(mat,x+1,y);
                printf("Tabuleiro atual:\n");
                print_mat(mat);

            }
            else if(sent == 'H' && (y-1) >= 0 && (y+1) < TAM && mat[x][y-1]=='~' && mat[x][y+1]=='~'){
                mat[x][y+1]='N';
                mat[x][y-1]='N';
                area_nav(mat,x,y-1);
                area_nav(mat,x,y+1);
                printf("Tabuleiro atual:\n");
                print_mat(mat);
            }
            else{
                i--; //repete a interação
                printf("Posição Invalida ou ocupada. Tente novamente\n");
                mat[x][y]='~';
                continue;
            }
        }
        //Depois o jogador coloca os navios de tamanho 2
        else if(i<6){
        	printf("Posicione um extremo do navio %d de tamanho 2 (ex: A 1): ", i + 1);
            scanf(" %c %d", &col, &y);
            fflush(stdin);
            putchar('\n');
            x = col - 'A'; // Converte a letra da coluna para índice numérico

            if(x >= 0 && x < TAM && y >= 0 && y < TAM && mat[x][y] == '~'){
            	mat[x][y]='N';
            }
            else {
            	printf("Posição inválida ou ocupada. Tente novamente.\n");
            	i--; // Repete a iteração
            	continue;
            }
            printf("Defina a direção do navio em Vertica(V) ou Horizontal(H): ");
            scanf(" %c", &sent);
            fflush(stdin);
            if(sent == 'V' || sent == 'v'){
            	printf("Define se o navio vai ficar para cima(C) ou para baixa(B):");
				scanf(" %c",&sent);
				fflush(stdin);
				if((sent == 'C' || sent == 'c') && mat[x-1][y] == '~' && x-1 >= 0){
                	mat[x-1][y]= 'N';
                	area_nav(mat,x-1,y);
                	area_nav(mat,x,y);
					printf("Tabuleiro atual:\n");
                	print_mat(mat);
                }
                else if((sent == 'B' || sent == 'b') && mat[x+1][y] == '~' && x+1 < TAM){
                	mat[x+1][y]= 'N';
                	area_nav(mat,x+1,y);
                	area_nav(mat,x,y);
                	printf("Tabuleiro atual: \n");
                	print_mat(mat);
				}
				else{
					i--; //repete a interação todinha
					printf("Posição Invalida ou ocupada. Tente novamente\n");
                	mat[x][y] = '~';
                	continue;
				}

            }
            else if(sent == 'H' || sent == 'h'){
                printf("Define se o navio vai ficar para a esquerda(E) ou para a direita(D):");
				scanf(" %c",&sent);
				fflush(stdin);
				if((sent == 'E' || sent == 'e') && mat[x][y-1] == '~' && y-1 >= 0){
                	mat[x][y-1]= 'N';
                	area_nav(mat,x,y-1);
                	area_nav(mat,x,y);
					printf("Tabuleiro atual:\n");
                	print_mat(mat);
                }
                else if((sent == 'D' || sent == 'd') && mat[x][y+1] == '~' && y+1 < TAM){
                	mat[x][y+1]= 'N';
                	area_nav(mat,x,y+1);
                	area_nav(mat,x,y);
                	printf("Tabuleiro atual: \n");
                	print_mat(mat);
				}
				else{
					i--; //repete a interação todinha
					printf("Posição Invalida ou ocupada. Tente novamente\n");
                	mat[x][y] = '~';
                	continue;
				}
            }
            else{
                i--; //repete a interação
                printf("Posição Invalida ou ocupada. Tente novamente\n");
                mat[x][y]='~';
                continue;
            }  
		}
		else{
			printf("Posicione o navio %d de tamanho 1 (ex: A 1): ", i + 1);
            scanf(" %c %d", &col, &y);
            fflush(stdin);
            putchar('\n');
            x = col - 'A'; // Converte a letra da coluna para índice numérico

            if(x >= 0 && x < TAM && y >= 0 && y < TAM && mat[x][y] == '~'){
            	mat[x][y]='N';
            	area_nav(mat,x,y);
            	print_mat(mat);	
            }
            else {
            	printf("Posição inválida ou ocupada. Tente novamente.\n");
            	i--; // Repete a iteração
            	continue;
            }
		}
    }
}

// Realiza um ataque na matriz do adversário
bool realizar_ataque(char mat_defesa[][TAM], char mat_ataque[][TAM], int x, int y){
    if(x >= 0 && x < TAM && y >= 0 && y < TAM){
        if(mat_defesa[x][y] == 'N'){
            mat_defesa[x][y] = 'X'; // 'X' representa um navio atingido
            mat_ataque[x][y] = 'X';  // Atualiza a matriz de ataque
            printf("Acertou!\n");
            return true;
        } else if(mat_defesa[x][y] == '~' || mat_defesa[x][y] == 'a'){
            mat_defesa[x][y] = 'O'; // 'O' representa um ataque na água
            mat_ataque[x][y] = 'O';  // Atualiza a matriz de ataque
            printf("Água!\n");
            return false;
        } else {
            printf("Posição já atacada.\n");
            return false;
        }
    }
    printf("Posição inválida.\n");
    return false;
}

// Verifica se todos os navios foram destruídos
bool todos_navios_destruidos(char mat[][TAM]){
    int i, j;
    for(i = 0; i < TAM; i++){
        for(j = 0; j < TAM; j++){
            if(mat[i][j] == 'N'){
                return false;
            }
        }
    }
    return true;
}

int main(){
    char mat_def1[TAM][TAM], mat_def2[TAM][TAM];
    char mat_atk1[TAM][TAM], mat_atk2[TAM][TAM];
    int x, y;
    char col;
    bool jogo_ativo = true;

    // Inicializa as matrizes
    inic_mat(mat_def1);
    inic_mat(mat_def2);
    inic_mat(mat_atk1);
    inic_mat(mat_atk2);

    // Posiciona os navios dos jogadores
    print_mat(mat_def1);
    printf("Jogador 1, posicione seus navios:\n");
    posicionar_navios(mat_def1);
    print_mat(mat_def2);
    printf("Jogador 2, posicione seus navios:\n");
    posicionar_navios(mat_def2);

    // Loop do jogo
    while(jogo_ativo){
        // Limpa a tela e mostra os tabuleiros antes do ataque
        system("clear");
        printf("Jogador 1 (Defesa)\t\tJogador 1 (Ataque)\n");
        print2_mat(mat_def1, mat_atk1);

        // Jogador 1 ataca
        printf("\nJogador 1, insira as coordenadas do ataque (ex: A 1): ");
        scanf(" %c%d", &col, &y);
        x = col - 'A';
        realizar_ataque(mat_def2, mat_atk1, x, y);
        if(todos_navios_destruidos(mat_def2)){
            printf("Jogador 1 venceu!\n");
            jogo_ativo = false;
            break;
        }

        // Limpa a tela e mostra os tabuleiros antes do ataque
        system("clear");
        printf("Jogador 2 (Defesa)\t\tJogador 2 (Ataques)\n");
        print2_mat(mat_def2, mat_atk2);

        // Jogador 2 ataca
        printf("\nJogador 2, insira as coordenadas do ataque (ex: A 1): ");
        scanf(" %c%d", &col, &y);
        x = col - 'A';
        realizar_ataque(mat_def1, mat_atk2, x, y);
        if(todos_navios_destruidos(mat_def1)){
            printf("Jogador 2 venceu!\n");
            jogo_ativo = false;
            break;
        }
    }

    return 0;
}
