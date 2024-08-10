#include <stdio.h>
#include <stdlib.h>    /* exit, malloc, calloc, etc. */
#include <string.h>
#include <getopt.h>    /* getopt */
#include <time.h>

#include <likwid.h>
#include "matriz.h"
#include "utils.h"

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
  LIKWID_MARKER_INIT;
  
  MatRow mRow_1, mRow_2, resMat;
  Vetor vet, res;
  
  /* =============== TRATAMENTO DE LINHA DE COMANDO =============== */

  if (argc < 2)
    usage(argv[0]);

  n = atoi(argv[1]);
  
  /* ================ FIM DO TRATAMENTO DE LINHA DE COMANDO ========= */
 
  srandom(20232);
      
  res = geraVetor (n, 0); // (real_t *) malloc (n*sizeof(real_t));
  resMat = geraMatRow(n, n, 0);
    
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

  LIKWID_MARKER_START("MultMatVetSem");
  time_t inicio = timestamp();
  multMatVet(mRow_1, vet, n, n, res);
  printf("%d,%lf", n, timestamp() - inicio);
  LIKWID_MARKER_STOP("MultMatVetSem");

  LIKWID_MARKER_START("MultMatMatSem");
  inicio = timestamp();
  multMatMat(mRow_1, mRow_2, n, resMat);
  printf(",%lf", timestamp() - inicio);
  LIKWID_MARKER_STOP("MultMatMatSem");

#ifdef _DEBUG_
  prnVetor(res, n);
  prnMat(resMat, n, n);
#endif /* _DEBUG_ */

  // prnVetor(res, n);
  // prnMat(resMat, n, n);

  memset(res, 0, n * sizeof(real_t));
  memset(resMat, 0, n * n * sizeof(real_t));

  LIKWID_MARKER_START("MultMatVetCom");
  inicio = timestamp();
  multMatVetLoopUnrollingAndJam(mRow_1, vet, n, n, res);
  printf(",%lf", timestamp() - inicio);
  LIKWID_MARKER_STOP("MultMatVetCom");

  LIKWID_MARKER_START("MultMatMatCom");
  inicio = timestamp();
  multMatMatLoopUnrollingJamAndBlocking(mRow_1, mRow_2, n, resMat);
  printf(",%lf\n", timestamp() - inicio);
  LIKWID_MARKER_STOP("MultMatMatCom");
    
#ifdef _DEBUG_
    prnVetor (res, n);
    prnMat (resMat, n, n);
#endif /* _DEBUG_ */

  liberaVetor ((void*) mRow_1);
  liberaVetor ((void*) mRow_2);
  liberaVetor ((void*) resMat);
  liberaVetor ((void*) vet);
  liberaVetor ((void*) res);
  LIKWID_MARKER_CLOSE;    
  return 0;
}

