import matplotlib.pyplot as plt
import pandas as pd
import sys
import os

def plot_graph(csv_file_original, csv_file_optimized, title, ylabel, logx=True, logy=False, legend_loc='left top'):
    df_original = pd.read_csv(csv_file_original)
    df_optimized = pd.read_csv(csv_file_optimized)
    
    fig, ax = plt.subplots(figsize=(12, 7))
    
    if logx:
        ax.set_xscale('log')
    if logy:
        ax.set_yscale('log')
    
    # Plot original data
    ax.plot(df_original.iloc[:, 0], df_original.iloc[:, 1], 'g-o', label='MatVet')
    ax.plot(df_original.iloc[:, 0], df_original.iloc[:, 2], 'r-o', label='MatMat')
    
    # Plot optimized data
    ax.plot(df_optimized.iloc[:, 0], df_optimized.iloc[:, 1], 'b-o', label='MatVetOpt')
    ax.plot(df_optimized.iloc[:, 0], df_optimized.iloc[:, 2], 'm-o', label='MatMatOpt')
    
    ax.set_xlabel('N (bytes)')
    ax.set_ylabel(ylabel)
    ax.set_title(title)
    ax.grid(True)
    ax.set_xlim(64, 11000)
    
    if legend_loc == 'left top':
        ax.legend(loc='upper left')
    elif legend_loc == 'right top':
        ax.legend(loc='upper right')
    elif legend_loc == 'right bottom':
        ax.legend(loc='lower right')
    elif legend_loc == 'center top':
        ax.legend(loc='upper center')
    
    plt.tight_layout()
    return fig

def main(original_dir, optimized_dir):
    output_dir = 'Graficos' #DIRETORIO DE SAIDA
    os.makedirs(output_dir, exist_ok=True)

    # Tempo
    fig1 = plot_graph(f'{original_dir}/Tempos.csv', f'{optimized_dir}/Tempos.csv', 
                      'Tempo', 'Tempo (ms)', logy=True)
    fig1.savefig(f'{output_dir}/Tempo.png')
    plt.close(fig1)
    
    # FLOPS_DP
    fig2 = plot_graph(f'{original_dir}/FLOPS_DP.csv', f'{optimized_dir}/FLOPS_DP.csv', 
                      'FLOPS DP', 'FLOPS DP [MFlops/s]', legend_loc='right top')
    fig2.savefig(f'{output_dir}/FLOPS_DP.png')
    plt.close(fig2)
    
    # L3
    fig3 = plot_graph(f'{original_dir}/L3.csv', f'{optimized_dir}/L3.csv', 
                      'L3', 'L3 [MBytes/s]')
    fig3.savefig(f'{output_dir}/L3.png')
    plt.close(fig3)
    
    # L2CACHE
    fig4 = plot_graph(f'{original_dir}/L2CACHE.csv', f'{optimized_dir}/L2CACHE.csv', 
                      'L2 miss ratio', 'L2 miss ratio', legend_loc='right bottom')
    fig4.savefig(f'{output_dir}/L2CACHE.png')
    plt.close(fig4)
    
    # ENERGY
    fig5 = plot_graph(f'{original_dir}/ENERGY.csv', f'{optimized_dir}/ENERGY.csv', 
                      'Energia', 'Energia [J]', legend_loc='center top')
    fig5.savefig(f'{output_dir}/ENERGY.png')
    plt.close(fig5)

    # FLOPS_AVX
    fig6 = plot_graph(f'{original_dir}/FLOPS_AVX.csv', f'{optimized_dir}/FLOPS_AVX.csv', 
                      'FLOPS AVX', 'FLOPS AVX [MFlops/s]', legend_loc='right top')
    fig6.savefig(f'{output_dir}/FLOPS_AVX.png')
    plt.close(fig6)
    
    print(f"Gráficos salvos no diretório '{output_dir}'.")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Uso: python script.py <original_data_directory> <optimized_data_directory>")
        sys.exit(1)
    
    original_dir = sys.argv[1]
    optimized_dir = sys.argv[2]
    main(original_dir, optimized_dir)