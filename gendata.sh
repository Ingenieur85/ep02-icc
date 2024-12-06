#!/bin/bash

PROG=matmult
CPU=3

export DATA_DIR="Resultados"

mkdir -p ${DATA_DIR}

echo "performance" > /sys/devices/system/cpu/cpufreq/policy${CPU}/scaling_governor

make purge
make

METRICA="FLOPS_DP L2CACHE L3 ENERGY"
TEMPOS="${DATA_DIR}/Tempos.csv"
TAMANHOS="64 128 200 256 512 600 900 1024 2000 2048 3000 4000 5000 6000 10000"

for m in ${METRICA}
do
    LIKWID_LOG="${DATA_DIR}/${m}.csv"
    rm -f ${LIKWID_LOG}
    rm -f ${TEMPOS}
    
    for n in $TAMANHOS
    do
	LIKWID_OUT="${DATA_DIR}/${m}_${n}.txt"
	rm -f ${LIKWID_OUT}
	
	echo "--->>  $m: ./${PROG} $n" #>/dev/tty
	likwid-perfctr -O -C ${CPU} -g ${m} -o ${LIKWID_OUT} -m ./${PROG} ${n} >>${TEMPOS}

	# A partir dos LOGS, gera arquivos CSV com 3 colunas: N, metrica_matvet, metrica_matmat
	# ./genplot.sh ${m} ${n} >> ${LIKWID_LOG}
	./genplot.py ${m} ${n} ${DATA_DIR} >> ${LIKWID_LOG}
    done

done

# Gera FLOPS_AVX, sem computar nada, simplesmente pega a metrica do FLOPS_DP já calculado
for n in $TAMANHOS
do
    # Copia FLOPS_DP para FLOPS_AVX
    cp "${DATA_DIR}/FLOPS_DP_${n}.txt" "${DATA_DIR}/FLOPS_AVX_${n}.txt"
    
    # Gera FLOPS_AVX.csv com o genplot.py original
    ./genplot.py FLOPS_AVX ${n} ${DATA_DIR} >> "${DATA_DIR}/FLOPS_AVX.csv"
done

# Consolida FLOPS_DP com FLOPS_AVX para gerar o grafico conjunto
#DP="${DATA_DIR}/FLOPS_DP.csv"
#AVX="${DATA_DIR}/FLOPS_AVX.csv"
#paste -d',' <(cut -d',' -f1-3 "$DP") <(cut -d',' -f2-3 "$AVX") > "${DATA_DIR}/FLOPS_CONSOLIDATED.csv"

echo "powersave" > /sys/devices/system/cpu/cpufreq/policy${CPU}/scaling_governor 
echo "Script terminado com sucesso!"
