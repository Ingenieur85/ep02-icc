# EP02 - Introdução à Computação Científica
Feito por Fabiano A. de Sá Filho

GRR: 20223831

09/08/2024

IMPORTANTE: O script de geração dos gráficos `plot.py` requer o uso de matplotlib e pandas!

## Descrição

Este trabalho teve como objetivo comparar e melhorar a performance de funções de multiplicação de matrizes. Para tal, implementei técnicas de otimização serial de código, e testei em multiplicações de matrizes aleatórias com tamanhos variados, no intervalo [64, 10000].

O código é instrumentado por Likwid, e gera gráficos que comparam as diversas métricas de performance entre os tamanhos e tipos de multiplicação: Matriz-Vetor(MatVet) e Matriz-Matriz(MatMat).

## Otimizações Implementadas

Em ambas as funções MatVet e MatMat, implementei as otimizações Unroll & Jam e Blocking; descrevo-as em mais detalhes nas subseções à seguir. Fiz algumas mudanças básicas, inspiradas na aula de "Medidas simples, impacto grande", como: 
1. Utilizar acumuladores para o laço interno, a fim de economizar os acessos à matriz C.
2. Calcular e armazenar os intervalos de U&J em variáveis a fim de economizar estas operações nas chamadas dos laços. 

Não implementei instruções de SIMD. Fiz alguns testes de *padding*, mas obtive falhas de segmentação ao testar. Optei, portanto, por entregar um *U&J* com *Blocking* bem feitos, e que produzem as soluções corretas, do que entregar técnicas extras que não tive o tempo de dominar. 

1. **Loop Blocking**: A ideia consiste em quebrar a matriz em blocos de tamanhos menores que caibam por completo na cache, e operar sobre um bloco de cada vez, diminuindo assim o número de operações de acesso a memória. Não fiz nenhum teste empírico com tamanhos de blocos variados para obter o tamanho adequado do bloco. Mas, com um cálculo rápido, podemos obter uma estimativa do bloco ideal: A cache L1 do computador utilizado (ver documento topology.txt, máquina H28 do Dinf) tem 32kb, implicando em um tamanho ideal de bloco de 32; isto por que a próxima potencia de 2, 64, produziria um bloco de 4096 doubles, ou seja, 4096*8 = 32768 bytes, o que excederia o tamanho da cache L1, diminuindo os efeitos positivos da blocagem na performance.

2. **Unroll & Jam**: A idéia do U&J é similar a de blocagem, onde neste caso queremos operar sobre dados que caibam em exatamente uma linha de cache. U&J agrupa operações que usam dados próximos na memória, maximizando o uso de cada linha de cache, e diminuindo *cache miss*. Na arquitetura utilizada, a linha de cache tem um tamanho de 64 bytes (ver cpuinfo.txt, *clflush size*), isto implica que podemos colocar até 8 doubles em uma linha de cache, significando que o fator ótimo de unroll é de 8. Para testar essa tese, efetuei testes de apenas U&J com os tamanhos até 512(ver diretorio `versoesMatriz`), e de fato, o fator de unroll 8 resulta em menores tempos de execução e um menor cache miss ratio.


Portanto, a configuração final mais otimizada foi a de um fator de Unroll de 8, e um fator de blocagem de 32, para ambas as funções MatVet e MatMult.


## Uso

Para compilar, faça:
~~~
make purge
make
~~~

Para utilizar o programa de multiplicação, e atestar que ele resolve corretamente as matrizes, inclua no arquivo `matriz.c`:
~~~
#define _DEBUG_ 1
~~~
e faça:
~~~
./matmult n
~~~
Onde n é a dimensão da matriz.

Para gerar as métricas de performance do Likwid para matrizes de diversos tamanhos, faça:
~~~
./gendata.sh
~~~
Este script faz as chamadas do executável, guardando as saídas do Likwid em aquivos `.txt` no diretório `Resultados`. Por fim, ele chama chama o script genplot.py para agregar os resultados em arquivos `.csv`. Note que para a métrica `FLOPS_AVX`, `gendata.sh` simplesmente copia os resultados já calculados em `FLOPS_DP` sem executar novamente, uma vez que `FLOPS_DP` já contém a métrica desejada.

Uma vez gerados os arquivos `.csv` chame o script `plot.py` para gerar os gráficos no diretório `Graficos`. Este script requer dois argumentos, que são caminhos para os diretórios contendo os resultados não otimizados e otimizados, nesta ordem. Veja:
~~~
python3 plot.py <dir_não_otimizado> <dir_otimizado>
~~~

Para gerar os gráficos contidos em `Graficos`, faça:
~~~
python3 plot.py NotOpt Resultados
~~~

Os resultados de NotOpt foram gerados com o arquivo `matriz.c` original, uma cópia do qual existe no diretório `versoesMatriz`.

## Discussão dos Resultados
A partir da análise dos gráficos, podemos obter algumas conclusões para cada uma das métricas.

1. **Tempo de Execução**:
Observando o gráfico `Tempo.png`, Podemos notar que o maior ganho de performance foi para a multiplicação matriz-matriz. A versão otimizada executa em uma ordem de magnitude mais rápida (10 vezes). Esta ordem de melhoria é praticamente constante entre os tamanhos de matriz, tendo um n=10000 aproximadamente a mesma melhora de 10 vezes se comparada à uma matriz de n=64. Já a multiplicação matriz-vetor exibe uma melhora bem mais modesta, porém melhora esta que parece aumentar conforme cresce o tamanho da matriz.

2. **Banda de Memória**: 
Os resultados de banda de memória para MatVet foram de acordo com o esperado; A versão otimizada exibe uma banda superior para todos os tamanhos de matriz, significado que  o código otimizado utiliza melhor os seus acessos à memória. Já para Mat-Mat, os resultados são inconsistentes, com a performance oscilando entre a versão otimizada(Opt) e não otimizada(nOpt). A nOpt exibe uma banda muito superior para as matrizes menores, com a Opt melhorando para matrizes maiores. Estas oscilações na banda de memória não são esperadas, e até o momento não descobri o seu motivo. 

3. **Cache miss L2**:
Esta métrica tenta capturar a porcentagem de vezes em que a informação solicitada pelo programa não se encontra na cache L2, e precisa ser buscada na L3 ou na RAM. Neste caso podemos observar que a versão otimizada é melhor, principalmente para matrizes menores.
É curioso notar que para o teste de n=2048 as duas versões otimizadas obtiveram miss ratios substancialmente inferiores as versões originais. Veja:

|Miss Ratio |    Mat-Vet    |    Mat-Mat    |
|:---------:|:-------------:|:-------------:|
|   notOpt    |    0.3000     |    0.3270     |
|    Opt    |    0.3955     |    0.4257     |

Com essa exceção à parte, as otimizações parecem oferecer um miss ratio menor para outros tamanhos de n, como mostra o gráfico `L2CACHE.png`.

4. **Energia**:
Neste teste podemos ver visualmente como a versão otimizada Mat-Mat é exponencialmente superior a versão original em sua economia de energia. Apesar de não ser visível no gráfico, Mat-Vet otimizado é também superior à sua contra-parte não otimizada. Para n=10000, Mat-Vet otimizada consome aproximadamente 0.6 J, contra 1.2 J, oferecendo uma melhora na ordem de 2 vezes.

5. **FLOPS_DP**:
Neste métrica, as versões otimizadas também são superiores para todos os tamanhos de matriz. Mat-Mat otimizada efetua, na média, cerca de 4x mais FLOP/s do que a não otimizada. Porém, sua performance é bem mais variável com grande volatilidade. Isto se deve ao *Cache Thrashing* causado pelas dimensões de matriz que são potências de 2, e fazem com que a velocidade das MFLOP/s caiam substancialmente para estas dimensões.
Como não efetuei operações com SIMD, não houveram medições de FLOPS_AVX diferentes de 0. Portanto, não criei a tabela consolidado com FLOPS_DP, como foi solicitado.


## Bugs e Limitações Conhecidas


- Por algum motivo, nos testes para n=100, obtive erros de segmentação e erros prematuros na execução do `./matmult`. Inicialmente, acreditei ser pelo fato de 100 não ser múltiplo do fator de unroll(8) ou de blocagem(32), mas não obtive nenhum outro erro desse gênero em testes com nenhum outro tamanho de matriz. Até o momento, não consegui identificar o motivo do programa falhar somente para o tamanho 100. Por essa razão, excluí a dimensão n=100 dos testes apresentados. 


# Dependências

Para executar este projeto:


- **Python 3.8+**
- **Matplotlib**
- **Pandas**

- O programa deve obrigatoriamente ser compilado e executado em uma máquina física rodando Linux AMD64. A biblioteca LIKWID deve estar devidamente instalada e acessível ao programa.




