#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//I.para armazenar a gray level dos bits da imagem, que variam de 0 a 255, o tipo mais econômico
//em C é o 'char', que varia entre [-128,+127]; mas como tem que converter isso, optei tipo int;
//II.a operação log ilumina significativamente os pixels mais escuros, contrastando-os com os mais
//claros. Se usada em imagens já bastante claras (i.e, bubbles2 ou sat_image) o processo gera 
//pgm's muito claros, basicamente indecifráveis;

typedef struct im{
  int x, y;
  int pM, pm;//pixel mais e menos significativo;
  int tipo;
  int *vetor_pixels;//armazenarei os pixels como vetores;
}imagem; 

void ler_ASCII (imagem *img_input, char *arquivo){
//essa função lê arquivos dados como P2;    
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

void ler_BIN (imagem *img_input, char *arquivo){
//essa função lê arquivos dados como P5;    
  FILE *temp;
  char aux;
  temp = fopen (arquivo, "rb");
  if (temp == NULL){
  printf("Erro em abrir o arquivo pra ler!\n");
  exit(1);
  }
  short unsigned int M=0, m=255;
  int tamanho = (*img_input).x * (*img_input).y;
  int i = 0;
  img_input->vetor_pixels = (int*) malloc ((tamanho+1)*sizeof(int));
  while(i<4){
    aux = fgetc(temp);
    if (aux == '\n'){
      i++;
    }
  }
  i = 0;
  while (feof(temp) == 0){
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
//essa função recebe o texto do usuário e extrai o header, decidindo como ler os pixels com base no tipo de arquivo;    
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
  aux = getw (input); 
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
//calcula o tamanho do vetor de pixels;    
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
//permite a aplicação do log sobre as imagens;    
  int pixel_maximo = img_input->pM;
  int i=0;
  int M=0, m=255;
  float c = 255/log(1+pixel_maximo);
  while (i<tamanho){
    img_input->vetor_pixels[i] = c*log(img_input->vetor_pixels[i]+1);
    if (img_input->vetor_pixels[i]>255){
        img_input->vetor_pixels[i] = 255;//impede que os dados passem de 255;
    }
    if (img_input->vetor_pixels[i] > M){
      M = img_input->vetor_pixels[i];
    }
    if (img_input->vetor_pixels[i] < m){
      m = img_input->vetor_pixels[i];
    }
    i++;
  }
  img_input->pM = M;
  img_input->pm = m;
}

void retornar_imagem (imagem *img_input, int tamanho){
//sempre devolve um ASCII;
  FILE *fp = NULL;
  fp = fopen("img_output.pgm", "w");
  if (fp == NULL){
    printf("Melou o arquivo!\n");
    exit(1);
  }
  char t1[] = "P2";
  char t2[] = "# CREATOR: Image Generator SCC-222 – Lab ICC I";
  fputs (t1, fp); 
  putc ('\n', fp);
  fputs (t2, fp);
  putc ('\n', fp);
  fprintf (fp, "%d %d\n", img_input->x, img_input->y);
  printf("maior pixel: %d\n", img_input->pM); //tem que colocar o NOVO valor!!
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
//testei o código em minha máquina e ele clareou perfeitamente o 'station.pgm' e o 'fusca.pgm' (a placa é EN 0968),
//falhando em melhorar a imagem somente nos outros dois arquivos. Acredito que isso seja devido à operação log, e não
//ao código em si (como já esclareci no começo, ela não contrasta bem imagens bastante claras);
