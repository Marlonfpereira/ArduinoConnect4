#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <LiquidCrystal.h>
#include <Adafruit_NeoPixel.h>

#define PIXEL_PIN    A8
#define PIXEL_COUNT 64
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5


Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

uint32_t background = strip.Color(0,0,0);
#define RED strip.Color(1,0,0)
#define YELLOW strip.Color(1,1,0)
#define GREEN strip.Color(0,1,0)
#define CYAN strip.Color(0,2,1)
#define BLUE strip.Color(0,0,1)
#define PINK strip.Color(1,0,1)
const uint32_t cores[] = {RED, YELLOW, GREEN, CYAN, BLUE, PINK};
const char nomeCores[10][10] = {"Vermelho", "Amarelo", "Verde", "Ciano", "Azul", "Rosa"};
short escolhida = -1;

int read_LCD_buttons()
{
 adc_key_in = analogRead(0);
 if (adc_key_in > 1000) return btnNONE;
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 return btnNONE;
}

typedef struct{
    uint32_t quadro[8][8];
    int posAtual;
    int nPartidas;
    uint32_t jogadores[2];
} partida;
partida jogo;

void limpaQuadro()
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
           jogo.quadro[i][j] = background;
    strip.clear();
    for(int i = 0; i < PIXEL_COUNT; i++)
      strip.setPixelColor(i, background);
    strip.show();
}

void resetaPos(uint32_t player)
{
    jogo.quadro[0][jogo.posAtual] = background;
    strip.setPixelColor(jogo.posAtual, background);
    jogo.posAtual = 0;
    jogo.quadro[0][jogo.posAtual] = player;
    strip.setPixelColor(jogo.posAtual, player);
}

void selecionar(short dir, uint32_t player)
{
    jogo.quadro[0][jogo.posAtual] = background;
    strip.setPixelColor(jogo.posAtual, background);
    jogo.posAtual += dir;
    if(jogo.posAtual < 0)
        jogo.posAtual = 7;
    else if(jogo.posAtual > 7)
        jogo.posAtual = 0;

    jogo.quadro[0][jogo.posAtual] = player;
    strip.setPixelColor(jogo.posAtual, player);
}
int turnos = 0;
int turno(uint32_t player);
int confirma(uint32_t player)
{
    int i = 1;
    while (jogo.quadro[i][jogo.posAtual] == background)
        i++;
    if(--i)
    {
        jogo.quadro[0][jogo.posAtual] = background;
        strip.setPixelColor(jogo.posAtual, background);
        jogo.quadro[i][jogo.posAtual] = player;
        if(i%2)
          strip.setPixelColor(i * 8 + (7-jogo.posAtual), player);
        else
          strip.setPixelColor(i*8+jogo.posAtual, player);
    } else
    {
        lcd.setCursor(3, 1);
        lcd.print(turnos++);
        turno(player);
    }

    return i;
}

int verifica(int linha, uint32_t player)
{
    int seq = 0;
    for (int i = 1; jogo.posAtual+i <= 7 && jogo.quadro[linha][jogo.posAtual+i] == player; i++)
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
            for (int i = 1; linha+i <= 7 && jogo.posAtual+i <= 7 && jogo.quadro[linha+i][jogo.posAtual+i] == player; i++)
                seq++;
            for (int i = 1; linha-i >= 1 && jogo.posAtual-i >= 0 && jogo.quadro[linha-i][jogo.posAtual-i] == player; i++)
                seq++;

            if (seq < 3)
            {
                seq = 0;
                for (int i = 1; linha+i <= 7 && jogo.posAtual-i >=0 && jogo.quadro[linha+i][jogo.posAtual-i] == player; i++)
                    seq++;
                for (int i = 1; linha-i >= 1 && jogo.posAtual+i <= 7 && jogo.quadro[linha-i][jogo.posAtual+i] == player; i++)
                    seq++;
            }
        }
    }

    if (seq >= 3)
        return 1;
    return 0;
}

int turno(uint32_t player)
{
    resetaPos(player);
    strip.show();

    int direcao;
    do {
        do {
          direcao = read_LCD_buttons();
        } while(direcao != btnRIGHT && direcao != btnLEFT && direcao != btnDOWN);
        delay(400);
        switch (direcao)
        {
        case btnRIGHT:
            selecionar(1, player);
            strip.show();
            break;

        case btnLEFT:
            selecionar(-1, player);
            strip.show();
            break;

        default:
            break;
        }
    } while (direcao != btnDOWN);

    if(verifica(confirma(player), player))
        return 1;
    return 0;
}

void animPisca(uint32_t cor) {
  strip.fill(cor * 3, 0, 64);
  strip.show();
  delay(250);
  strip.fill(cor, 0, 64);
  strip.show();
  delay(250);

  strip.fill(cor * 3, 0, 64);
  strip.show();
  delay(250);
  strip.fill(cor, 0, 64);
  strip.show();
  delay(250);
}

void escolheCores(short jogador) {
  int direcao, selec = 0;
  strip.clear();
  strip.show();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Escolha sua cor!");

  do {
      do {
        direcao = read_LCD_buttons();
      } while(direcao != btnRIGHT && direcao != btnLEFT && direcao != btnDOWN);
      delay(400);

      switch (direcao)
      {
        case btnRIGHT:
            selec++;
            if (selec == escolhida)
                selec++;
            if (selec > 5)
                selec = 0;
            break;

        case btnLEFT:
            selec--;
            if (selec == escolhida)
                selec--;
            if (selec < 0)
                selec = 5;
            break;

        default:
            break;
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      jogador? lcd.print("Jogador 2") : lcd.print("Jogador 1");
      lcd.setCursor(0, 1);
      lcd.print(nomeCores[selec]);
      strip.clear();
      strip.fill(cores[selec], 0, 64);
      strip.show();

  } while (direcao != btnDOWN);
  jogo.jogadores[jogador] = cores[selec];
  escolhida = selec;

  animPisca(cores[selec]);
}

void numPartidas(){
  short sel = 99, npart = 1;
  strip.clear();
  strip.show();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect4");
  lcd.setCursor(0,1);
  lcd.print("NPartidas:");
  do
  {
    lcd.setCursor(12, 1);
    lcd.print(npart);
    do
      sel = read_LCD_buttons();
    while(sel == btnNONE);
    delay(400);
    switch (sel)
    {
    case btnRIGHT:
      npart++;
      break;

    case btnLEFT:
      npart--;
      break;

    default:
      break;
    }
    if(npart <= 0)
      npart = 8;
    else if (npart >= 9)
      npart = 1;
  }while(sel == btnRIGHT || sel == btnLEFT);
  jogo.nPartidas = npart;
}

void setup()
{
  strip.begin();
  strip.clear();
  strip.show();

  lcd.begin(16,2);

  escolheCores(0);
  escolheCores(1);
  jogo.nPartidas = 1;
}



void loop()
{
  numPartidas();

  lcd.setCursor(12,0);
  lcd.print("J1J2");
  lcd.setCursor(12,1);
  lcd.print("0 0");
  short j1w = 0, j2w = 0;
  for (int partida = 0; partida < jogo.nPartidas; partida++)
    {
        limpaQuadro();
        int jogada = 1;
        while (++jogada)
        {
            lcd.setCursor(0,1);
            lcd.print(jogada % 2 ? "Jogador 2": "Jogador 1");
            if (turno(jogo.jogadores[jogada % 2]))
            {
                if (jogada % 2)
                {
                  j2w++;
                  lcd.setCursor(14,1);
                  lcd.print(j2w);
                } else
                {
                  j1w++;
                  lcd.setCursor(12,1);
                  lcd.print(j1w);
                }
                animPisca(jogo.jogadores[jogada%2]);
                animPisca(jogo.jogadores[jogada%2]);
                jogada = -1;
            } else if (jogada == 55)
            {
                printf("empate\n");
                animPisca(strip.Color(1, 1, 1));
                jogada = -1;
            }
        }
    }
}