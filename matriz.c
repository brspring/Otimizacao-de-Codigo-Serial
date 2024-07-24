#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // Para uso de função 'memset()'

#include "matriz.h"

/**
 * Função que gera valores para para ser usado em uma matriz
 * @param i,j coordenadas do elemento a ser calculado (0<=i,j<n)
*  @return valor gerado para a posição i,j
  */
static inline real_t generateRandomA( unsigned int i, unsigned int j)
{
  static real_t invRandMax = 1.0 / (real_t)RAND_MAX;
  return ( (i==j) ? (real_t)(BASE<<1) : 1.0 )  * (real_t)random() * invRandMax;
}

/**
 * Função que gera valores aleatórios para ser usado em um vetor
 * @return valor gerado
 *
 */
static inline real_t generateRandomB( )
{
  static real_t invRandMax = 1.0 / (real_t)RAND_MAX;
  return (real_t)(BASE<<2) * (real_t)random() * invRandMax;
}



/* ----------- FUNÇÕES ---------------- */

/**
 *  Funcao geraMatRow: gera matriz como vetor único, 'row-oriented'
 * 
 *  @param m     número de linhas da matriz
 *  @param n     número de colunas da matriz
 *  @param zerar se 0, matriz  tem valores aleatórios, caso contrário,
 *               matriz tem valores todos nulos
 *  @return  ponteiro para a matriz gerada
 *
 */

MatRow geraMatRow (int m, int n, int zerar)
{
  MatRow matriz = (real_t *) malloc(m*n*sizeof(real_t));

  if (matriz) {
    if (zerar)
      memset(matriz,0,m*n*sizeof(real_t));
    else
      for (int i=0; i < m; ++i)
	for (int j=0; j < n; ++j)
	  matriz[i*n + j] = generateRandomA(i, j);
  }
  
  return (matriz);
}


/**
 *  Funcao geraVetor: gera vetor de tamanho 'n'
 * 
 *  @param n  número de elementos do vetor
 *  @param zerar se 0, vetor  tem valores aleatórios, caso contrário,
 *               vetor tem valores todos nulos
 *  @return  ponteiro para vetor gerado
 *
 */

Vetor geraVetor (int n, int zerar)
{
  Vetor vetor = (real_t *) malloc(n*sizeof(real_t));

  if (vetor) {
    if (zerar)
      memset(vetor,0,n*sizeof(real_t));
    else
      for (int i=0; i < n; ++i)
	vetor[i] = generateRandomB();
  }
  
  return (vetor);
}

/**
 *  \brief: libera vetor
 * 
 *  @param  ponteiro para vetor
 *
 */
void liberaVetor (void *vet)
{
	free(vet);
}


/**
 *  Funcao multMatVet:  Efetua multiplicacao entre matriz 'mxn' por vetor
 *                       de 'n' elementos
 *  @param mat matriz 'mxn'
 *  @param m número de linhas da matriz
 *  @param n número de colunas da matriz
 *  @param res vetor que guarda o resultado. Deve estar previamente alocado e com
 *             seus elementos inicializados em 0.0 (zero)
 *  @return vetor de 'm' elementos
 *
 */

void multMatVet (MatRow mat, Vetor v, int m, int n, Vetor res)
{
    
  /* Efetua a multiplicação */
  if (res) {
    for (int i=0; i < m; ++i)
      for (int j=0; j < n; ++j)
        res[i] += mat[n*i + j] * v[j];
  }
}

void multMatVetLoopUnrollingAndJam (MatRow mat, Vetor v, int m, int n, Vetor res)
{
  if (res) {
    int f = 2; //fator de desenrolamento

    //loop unrolling com fator 2 
    for (int i=0; i < m-m%f; i+=f)
    {
      for (int j=0; j < n; ++j){
        res[i] += mat[n*i + j] * v[j];
        // aqui teriam mais conforme o fator
        res[i+(f-1)] += mat[n*(i+(f-1)) + (f-1)] * v[j];
      }
    }

    // calculo do residuo
    for (int i=m-m%f; i < m; i++)
      for (int j=0; j < n; ++j)
        res[i] += mat[n*i + j] * v[j];
  }
}

void multMatVetLoopUnrollingJamAndBlocking (MatRow mat, Vetor v, int m, int n, Vetor res)
{
  if (res) {
    int f = 2; //fator de desenrolamento
    int b = 4;

    //loop unrolling com fator 2 
    for (int ii=0; ii<m/b; ++ii) {
    istart=ii*b; 
    iend=istart+b;
      for (int jj=0; jj<n/b; ++jj) {
        jstart=jj*b; 
        jend=jstart+b;
        for (int i=istart; i < iend; i+=f)
        {
          for (int j=jstart; j < jend; ++j){
            res[i] += mat[n*i + j] * v[j];
            // aqui teriam mais conforme o fator
            res[i+(f-1)] += mat[n*(i+(f-1)) + (f-1)] * v[j];
          }
        }
      }
    }

    // calculo do residuo
    for (int i=m-m%f; i < m; i++)
      for (int j=0; j < n; ++j)
        res[i] += mat[n*i + j] * v[j];
  }
}

/**
 *  Funcao multMatMat: Efetua multiplicacao de duas matrizes 'n x n' 
 *  @param A matriz 'n x n'
 *  @param B matriz 'n x n'
 *  @param n ordem da matriz quadrada
 *  @param C   matriz que guarda o resultado. Deve ser previamente gerada com 'geraMatPtr()'
 *             e com seus elementos inicializados em 0.0 (zero)
 *
 */

void multMatMat (MatRow A, MatRow B, int n, MatRow C)
{

  /* Efetua a multiplicação */
  for (int i=0; i < n; ++i)
    for (int j=0; j < n; ++j)
      for (int k=0; k < n; ++k)
	      C[i*n+j] += A[i*n+k] * B[k*n+j];
}

void multMatMatLoopUnrollingAndJam (MatRow A, MatRow B, int n, MatRow C)
{
  int f = 2;  //fator 

  // unroll and jam com fator 2
  for (int i=0; i < n; ++i){
    for (int j=0; j < n-n%f; j+=f){
      C[i*n+j] =  0.0;
      C[i*n+(j+ (f-1))] = 0.0;
      for (int k=0; k < n; ++k){
	      C[i*n+j] += A[i*n+k] * B[k*n+j];
        // aqui teriam mais conforme o fator
        C[i*n+(j+(f-1))] += A[i*n+k] * B[k*n+(j+(f-1))];
      }
    }

  // residuo do laco J
  for (int j= n - n%f; j < n; ++j){
      C[i*n+j] = 0.0;
      for (int k=0; k < n; ++k){
	      C[i*n+j] += A[i*n+k] * B[k*n+j];
      }
    }
  }
}

void multMatMatLoopUnrollingJamAndBlocking (MatRow A, MatRow B, int n, MatRow C)
{
  int f = 2;  //fator 
  int b = 4;  // qunatos blocos a matriz sera dividida
  // qunado b é multiplo de n e n é multiplo de b n tem residuo
  int istart, jstart, kstart;
  int iend, jend, kend;
  // unroll and jam + blocking
  for (int ii=0; ii<n/b; ++ii) {
    istart=ii*b; 
    iend=istart+b;
    for (int jj=0; jj<n/b; ++jj) {
      jstart=jj*b; 
      jend=jstart+b;
      for (int kk=0; kk<n/b; ++kk) {
        kstart=kk*b; 
        kend=kstart+b;
          for (int i=istart; i < iend; ++i){
            for (int j=jstart; j < jend; j+=f){
              C[i*n+j] =  0.0;
              C[i*n+(j+ (f-1))] = 0.0;
              for (int k=kstart; k < kend; ++k){
                C[i*n+j] += A[i*n+k] * B[k*n+j];
                // aqui teriam mais conforme o fator
                C[i*n+(j+(f-1))] += A[i*n+k] * B[k*n+(j+(f-1))];
              }
            }
          }
      }
    }
  }
}



/**
 *  Funcao prnMat:  Imprime o conteudo de uma matriz em stdout
 *  @param mat matriz
 *  @param m número de linhas da matriz
 *  @param n número de colunas da matriz
 *
 */

void prnMat (MatRow mat, int m, int n)
{
  for (int i=0; i < m; ++i) {
    for (int j=0; j < n; ++j)
      printf(DBL_FIELD, mat[n*i + j]);
    printf("\n");
  }
  printf(SEP_RES);
}

/**
 *  Funcao prnVetor:  Imprime o conteudo de vetor em stdout
 *  @param vet vetor com 'n' elementos
 *  @param n número de elementos do vetor
 *
 */

void prnVetor (Vetor vet, int n)
{
  for (int i=0; i < n; ++i)
    printf(DBL_FIELD, vet[i]);
  printf(SEP_RES);
}

