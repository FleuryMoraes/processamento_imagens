#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//I.para armazenar a gray level dos bits da imagem, que variam de 0 a 255, o tipo mais econômico em C é o 'char', que varia entre [-128,+127]; mas como tem que converter isso, optei tipo int;
//II.explicar pq 'log' nem sempre resolve o problema pras imagens;  FALAR Q USOU FLOAT
//LEMBRAR: free;

typedef struct im{
  int x, y;
  int pM, pm;//pixel mais e menos significativo;
  int tipo;
  int *vetor_pixels;
}imagem; 

void ler_ASCII (imagem *img_input, char *arquivo){
  FILE *temp;
  char aux;
  temp = fopen (arquivo, "r");
  if (temp == NULL){
    printf("Erro em abrir o arquivo pra ler!\n");
    exit(1);
  }
  int tamanho = img_input->x * img_input->y;
  int i = 0;
  short unsigned int M=0, m=255;
  img_input->vetor_pixels = (int*) malloc ((tamanho+1)*sizeof(int));
  //MOVER FTELL PRA ONDE TEM NUMERO!!
  while(i<4){
    aux = fgetc(temp);
    if (aux == '\n'){
      i++;
    }
  }
  i = 0;
  while (feof(temp) == 0){
    fscanf(temp,"%d",&(img_input->vetor_pixels[i]));
    if (img_input->vetor_pixels[i] > M){
      M = img_input->vetor_pixels[i];
    }
    if (img_input->vetor_pixels[i] < m){
      m = img_input->vetor_pixels[i];
    }
    i++;
  }
  img_input->vetor_pixels[i] = 500;
  img_input->pM = M;
  img_input->pm = m;
  fclose (temp);
}

void ler_BIN (imagem *img_input, char *arquivo){//CORRIGIR DPS PRA ESSE CASO;
  FILE *temp;
  char aux;
  temp = fopen (arquivo, "rb");
  if (temp == NULL){
  printf("Erro em abrir o arquivo pra ler!\n");
  exit(1);
  }
  int tamanho = (*img_input).x * (*img_input).y;
  int i = 0;
  int M=0, m=255;
  img_input->vetor_pixels = (int*) malloc ((tamanho+1)*sizeof(int));
  while(i<4){
    aux = fgetc(temp);
    if (aux == '\n'){
      i++;
    }
  }
  i = 0;
  while (feof(temp) == 0){//o numero de caracteres está certo, porém está lendo gibberish??;
    fread(&(img_input->vetor_pixels[i]),sizeof(__uint8_t),1,temp);//tinha q usar 1 byte!;
    if (img_input->vetor_pixels[i] > M){
      M = img_input->vetor_pixels[i];
    }
    if (img_input->vetor_pixels[i] < m){
      m = img_input->vetor_pixels[i];
    }
    i++;
  }
  img_input->vetor_pixels[i] = 500;
  img_input->pM = M;
  img_input->pm = m;
  fclose (temp);
}

void receber_imagem (imagem *img_input){
  FILE *input;
  char aux;
  char *entrada;
  entrada = (char*) malloc (sizeof(char)*100);
  if (entrada == NULL){
    printf("erro na alocação dinâmica!\n");
    exit(1);
  }
  printf ("Digite o arquivo que você deseja analizar: ");
  scanf ("%s", entrada);
  input = fopen (entrada, "r");
  if (input == NULL){
    printf("Melou o arquivo!\n");
    exit(1);
  }
  fseek (input, 1, SEEK_SET);
  img_input->tipo = fgetc(input); //está armazenado como ascii;
  int count=0;
  while (count<2){//chegar na 3°linha do file;
    aux = fgetc(input);
    if (aux == '\n'){
      count++;
    } 
  }
  fscanf(input,"%d",&(img_input->x));
  fscanf(input,"%d",&(img_input->y));
  aux = getw (input); //pra pular a linha;
  fclose (input);
  if ((img_input)->tipo == 50){
    ler_ASCII (img_input, entrada);
    free (entrada);
  }else{
    ler_BIN (img_input, entrada);
    free (entrada);
  }
}

int vector_length (imagem *img_input){
  int size;
  for (size=0; ;size++){
    if (img_input->vetor_pixels[size] == 500){ //demarca o fim;
      break;
    }  
  }
  size++;
  return size;
}

void aplicar_log (imagem *img_input, int tamanho){
  int pixel_maximo = img_input->pM;
  int i=0;
  float c = 255/log(1+pixel_maximo);
  while (i<tamanho){
    img_input->vetor_pixels[i] = c*log(img_input->vetor_pixels[i]+1);
    i++;
  }
}

void retornar_imagem (imagem *img_input, int tamanho){
  FILE *fp = NULL;
  fp = fopen("img_output.pgm", "w");
  if (fp == NULL){
    printf("Melou o arquivo!\n");
    exit(1);
  }
  char t1[] = "P5";
  char t11[] = "P2";
  char t2[] = "# CREATOR: Image Generator SCC-222 – Lab ICC I";
  if (img_input->tipo == 53){
    fputs (t1, fp);
  }else{
    fputs (t11, fp);
  }
  putc ('\n', fp);
  fputs (t2, fp);
  putc ('\n', fp);
  fprintf (fp, "%d %d\n", img_input->x, img_input->y);
  printf("maior pixel: %d\n", img_input->pM);//leu 222?
  fprintf (fp, "%d\n", img_input->pM);
  int i = 0;
  char aux;
  while (i<tamanho){
    fprintf(fp,"%d",img_input->vetor_pixels[i]); 
    putc('\n', fp);
    i++;
  }
  fclose (fp);
}

int main (void){
  int tamanho;
  imagem img_input;
  receber_imagem (&img_input);
  tamanho = vector_length(&img_input);
  aplicar_log (&img_input, tamanho);
  retornar_imagem (&img_input, tamanho);
  free(img_input.vetor_pixels);
  return 0;
}
//revisar: ponteiros e structs, free, inicializar tudo com NULL, arquivos (files em funções e tals), 
//strings; Pq tava dando errado com a função devolver um ponteiro pra file??
//checar tbm: GDB (comandos e extensao pro vscode) & makefile;
//PARAR de ser bobo e aprender github comandos no terminal;