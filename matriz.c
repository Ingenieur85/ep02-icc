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


#define UNRL 8
#define BLOCK_SIZE 32

void multMatVet(MatRow mat, Vetor v, int m, int n, Vetor res)
{
    int i, j, ii, jj;
    int m_mod = m % UNRL;
    int m_minus_m_mod = m - m_mod;


    for (ii = 0; ii < m_minus_m_mod; ii += BLOCK_SIZE) {
        int iend = ii + BLOCK_SIZE < m_minus_m_mod ? ii + BLOCK_SIZE : m_minus_m_mod;
        for (jj = 0; jj < n; jj += BLOCK_SIZE) {
            int jend = jj + BLOCK_SIZE < n ? jj + BLOCK_SIZE : n;

            for (i = ii; i < iend; i += UNRL) {
                for (j = jj; j < jend; ++j) {
                    res[i]   += mat[i*n + j]     * v[j];
                    res[i+1] += mat[(i+1)*n + j] * v[j];
                    res[i+2] += mat[(i+2)*n + j] * v[j];
                    res[i+3] += mat[(i+3)*n + j] * v[j];
                    res[i+4] += mat[(i+4)*n + j] * v[j];
                    res[i+5] += mat[(i+5)*n + j] * v[j];
                    res[i+6] += mat[(i+6)*n + j] * v[j];
                    res[i+7] += mat[(i+7)*n + j] * v[j];
                }
            }
        }
    }

    // Resolve o resto
    for (i = m_minus_m_mod; i < m; ++i) {
        for (j = 0; j < n; ++j) {
            res[i] += mat[i*n + j] * v[j];
        }
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

#define UNRL 8
#define BLOCK_SIZE 32

void multMatMat(MatRow A, MatRow B, int n, MatRow C)
{
    int i, j, k, ii, jj, kk;
    int istart, iend, jstart, jend, kstart, kend;

    for (ii = 0; ii < n; ii += BLOCK_SIZE) {
        istart = ii;
        iend = (ii + BLOCK_SIZE < n) ? ii + BLOCK_SIZE : n;
        for (jj = 0; jj < n; jj += BLOCK_SIZE) {
            jstart = jj;
            jend = (jj + BLOCK_SIZE < n) ? jj + BLOCK_SIZE : n;
            for (kk = 0; kk < n; kk += BLOCK_SIZE) {
                kstart = kk;
                kend = (kk + BLOCK_SIZE < n) ? kk + BLOCK_SIZE : n;

                for (i = istart; i < iend; ++i) {
                    for (j = jstart; j < jend; j += UNRL) {
                        double sum0 = 0.0, sum1 = 0.0, sum2 = 0.0, sum3 = 0.0;
                        double sum4 = 0.0, sum5 = 0.0, sum6 = 0.0, sum7 = 0.0;
                        for (k = kstart; k < kend; ++k) {
                            double a_ik = A[i*n + k];
                            sum0 += a_ik * B[k*n + j];
                            sum1 += a_ik * B[k*n + j + 1];
                            sum2 += a_ik * B[k*n + j + 2];
                            sum3 += a_ik * B[k*n + j + 3];
                            sum4 += a_ik * B[k*n + j + 4];
                            sum5 += a_ik * B[k*n + j + 5];
                            sum6 += a_ik * B[k*n + j + 6];
                            sum7 += a_ik * B[k*n + j + 7];
                        }
                        //Acumulador reduz acesso a memoria de C[]
                        C[i*n + j] += sum0;
                        C[i*n + j + 1] += sum1;
                        C[i*n + j + 2] += sum2;
                        C[i*n + j + 3] += sum3;
                        C[i*n + j + 4] += sum4;
                        C[i*n + j + 5] += sum5;
                        C[i*n + j + 6] += sum6;
                        C[i*n + j + 7] += sum7;
                    }
                }
            }
        }
    }

    // Resolve o resto do loop Blocking
    for (i = 0; i < n; ++i) {
        for (j = (n / UNRL) * UNRL; j < n; ++j) {
            for (k = 0; k < n; ++k) {
                C[i*n + j] += A[i*n + k] * B[k*n + j];
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

