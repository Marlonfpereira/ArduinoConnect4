#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct{
    char quadro[8][7];
    int posAtual;
    int nPartidas;
    char jogadores[2];
} partida;
partida jogo;

void limpaQuadro()
{
    memset(jogo.quadro, '*', 56);
    memset(jogo.quadro, '-', 7);
}

void mostraQuadro()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 7; j++)
            printf("%2c", jogo.quadro[i][j]);
        printf("\n");
    }
        printf("\n");
}

void resetaPos(char player)
{
    jogo.quadro[0][jogo.posAtual] = '-';
    jogo.posAtual = 0;
    jogo.quadro[0][jogo.posAtual] = player;
}

void selecionar(short dir, char player)
{
    jogo.quadro[0][jogo.posAtual] = '-';
    jogo.posAtual += dir;
    if(jogo.posAtual < 0)
        jogo.posAtual = 6;
    else if(jogo.posAtual > 6)
        jogo.posAtual = 0;

    jogo.quadro[0][jogo.posAtual] = player;
}

int turno(char player);
int confirma(char player)
{
    int i = 1;
    while (jogo.quadro[i][jogo.posAtual] == '*')
        i++;
    if(--i)
    {
        jogo.quadro[0][jogo.posAtual] = '-';
        jogo.quadro[i][jogo.posAtual] = player;
    } else
        turno(player);

    return i;
}

int verifica(int linha, char player)
{
    int seq = 0;
    for (int i = 1; jogo.posAtual+i <= 6 && jogo.quadro[linha][jogo.posAtual+i] == player; i++)
        seq++;
    for (int i = 1; jogo.posAtual-i >= 0 && jogo.quadro[linha][jogo.posAtual-i] == player; i++)
        seq++;

    if (seq < 3)
    {
        seq = 0;
        for (int i = 1; linha+i <= 7 && jogo.quadro[linha+i][jogo.posAtual] == player; i++)
            seq++;
        for (int i = 1; linha-i >= 1 && jogo.quadro[linha-i][jogo.posAtual] == player; i++)
            seq++;

        if (seq < 3)
        {
            seq = 0;
            for (int i = 1; linha+i <= 7 && jogo.posAtual+i <= 6 && jogo.quadro[linha+i][jogo.posAtual+i] == player; i++)
                seq++;
            for (int i = 1; linha-i >= 1 && jogo.posAtual-i >= 0 && jogo.quadro[linha-i][jogo.posAtual-i] == player; i++)
                seq++;

            if (seq < 3)
            {
                seq = 0;
                for (int i = 1; linha+i <= 7 && jogo.posAtual-i >=0 && jogo.quadro[linha+i][jogo.posAtual-i] == player; i++)
                    seq++;
                for (int i = 1; linha-i >= 1 && jogo.posAtual+i <= 6 && jogo.quadro[linha-i][jogo.posAtual+i] == player; i++)
                    seq++;
            }
        }
    }

    if (seq >= 3)
        return 1;
    return 0;
}

int turno(char player)
{
    resetaPos(player);
    mostraQuadro();
    printf("Jogador: %c (A/S/D)\n", player);
    char direcao = '\0';

    do {
        scanf("%[^'\n']c", &direcao);
        getchar();
        switch (direcao)
        {
        case 'd':
            selecionar(1, player);
            mostraQuadro();
            break;

        case 'a':
            selecionar(-1, player);
            mostraQuadro();
            break;

        default:
            break;
        }
    } while (direcao != 's');

    if(verifica(confirma(player), player))
        return 1;
    return 0;
}

int main()
{
    jogo.jogadores[0] = '+';
    jogo.jogadores[1] = 'X';
    printf("N de partidas:\n");
    scanf("%d", &jogo.nPartidas);
    char vitorias[jogo.nPartidas];

    for (int partida = 0; partida < jogo.nPartidas; partida++)
    {
        limpaQuadro();
        int jogada = 0;
        while (++jogada)
        {
            if (turno(jogo.jogadores[jogada % 2]))
            {
                printf("vencedor = %c\n", jogo.jogadores[jogada%2]);
                vitorias[partida] = jogo.jogadores[jogada % 2];
                jogada = -1;
            } else if (jogada == 14)
            {
                vitorias[partida] = 'E';
                printf("empate\n");
                jogada = -1;
            }
        }
    }

    for (int partida = 0; partida < jogo.nPartidas; partida++)
        printf("%c ", vitorias[partida]);

    return 0;
}