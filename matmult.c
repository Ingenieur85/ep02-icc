#include <stdio.h>
#include <stdlib.h>    /* exit, malloc, calloc, etc. */
#include <string.h>
#include <getopt.h>    /* getopt */
#include <time.h>

#include <likwid.h>
#include "utils.h"

#include "matriz.h"


/**
 * Exibe mensagem de erro indicando forma de uso do programa e termina
 * o programa.
 */

static void usage(char *progname)
{
  fprintf(stderr, "Forma de uso: %s [ <ordem> ] \n", progname);
  exit(1);
}



/**
 * Programa principal
 * Forma de uso: matmult [ -n <ordem> ]
 * -n <ordem>: ordem da matriz quadrada e dos vetores
 *
 */

int main (int argc, char *argv[]) 
{
  int n=DEF_SIZE;
  
  MatRow mRow_1, mRow_2, resMat;
  Vetor vet, res;
  
  /* =============== TRATAMENTO DE LINHA DE COMANDO =============== */

  if (argc < 2)
    usage(argv[0]);

  n = atoi(argv[1]);
  
  /* ================ FIM DO TRATAMENTO DE LINHA DE COMANDO ========= */
 
  srandom(20232);
      
  res = geraVetor (n, 0); // (real_t *) malloc (n*sizeof(real_t));
  resMat = geraMatRow(n, n, 1);
    
  mRow_1 = geraMatRow (n, n, 0);
  mRow_2 = geraMatRow (n, n, 0);

  vet = geraVetor (n, 0);

  if (!res || !resMat || !mRow_1 || !mRow_2 || !vet) {
    fprintf(stderr, "Falha em alocação de memória !!\n");
    liberaVetor ((void*) mRow_1);
    liberaVetor ((void*) mRow_2);
    liberaVetor ((void*) resMat);
    liberaVetor ((void*) vet);
    liberaVetor ((void*) res);
    exit(2);
  }
    
#ifdef _DEBUG_
    prnMat (mRow_1, n, n);
    prnMat (mRow_2, n, n);
    prnVetor (vet, n);
    printf ("=================================\n\n");
#endif /* _DEBUG_ */

/**** MODIFICAÇÕES  *****/
double time_matvet = 0.0, time_matmat = 0.0;


  LIKWID_MARKER_INIT;

/* MAT VET*/
  LIKWID_MARKER_START("multMatVet");
  time_matvet = timestamp();
  multMatVet (mRow_1, vet, n, n, res);
  time_matvet = timestamp() - time_matvet;
  LIKWID_MARKER_STOP("multMatVet");

/* MAT MAT*/
  LIKWID_MARKER_START("multMatMat");
  time_matmat = timestamp();
  multMatMat (mRow_1, mRow_2, n, resMat);
  time_matmat = timestamp() - time_matmat;
  LIKWID_MARKER_STOP("multMatMat");
  
  LIKWID_MARKER_CLOSE;
  /* LOG TIMES*/
  FILE *fp;
  fp = fopen("Resultados/Tempos.csv", "a");

  if (!fp) {
    perror("Não abriu o log dos tempos");
    return -1;
  }

  fprintf(fp, "%d, %.4f,%.4f\n", n, time_matvet, time_matmat);
  fclose(fp);


/************************/

#ifdef _DEBUG_
    prnVetor (res, n);
    prnMat (resMat, n, n);
#endif /* _DEBUG_ */

  liberaVetor ((void*) mRow_1);
  liberaVetor ((void*) mRow_2);
  liberaVetor ((void*) resMat);
  liberaVetor ((void*) vet);
  liberaVetor ((void*) res);

  return 0;
}

