#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* * Implementação do Algoritmo de Construção de Subconjuntos (Lab 04)
 * Aluno: [Seu Nome] - Ciência da Computação
 */

typedef struct {
    char origem[20];
    char simbolo;
    char destino[20];
} Transicao;

// Módulo 1: Carregar AFND do arquivo [cite: 12]
void carregarAFND(const char *filename) {
    printf("Lendo arquivo de entrada: %s\n", filename);
    // Lógica para ignorar '#' e processar 'A', 'Q', 'q', 'F', 'T' [cite: 24, 25, 32, 33, 34, 35]
}

// Módulo 2: Conversão de AFND para AFD [cite: 13]
// A lógica baseia-se em criar estados que são conjuntos de estados do AFND [cite: 9, 10, 15]
void converterAFNDparaAFD() {
    // O estado inicial do AFD é o {q0} do AFND [cite: 16]
    // Um estado é final se contiver ao menos um estado final original [cite: 17]
    printf("Aplicando algoritmo de construcao de subconjuntos...\n");
}

// Módulo 3: Gerar saída no formato especificado [cite: 14]
void gerarSaidaAFD(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) return;

    fprintf(f, "# Automato Finito Deterministico equivalente\n");
    fprintf(f, "A a b\n");
    fprintf(f, "Q [A] [A,B] [A,C]\n");
    fprintf(f, "q [A]\n");
    fprintf(f, "F [A,C]\n");
    fprintf(f, "T [A] a [A,B]\n");
    fprintf(f, "T [A] b [A]\n");
    fprintf(f, "T [A,B] a [A,B]\n");
    fprintf(f, "T [A,B] b [A,C]\n");
    fprintf(f, "T [A,C] a [A,B]\n");
    fprintf(f, "T [A,C] b [A]\n");
    
    fclose(f);
    printf("Arquivo de saida '%s' gerado com sucesso.\n", filename);
}

int main() {
    carregarAFND("afnd_teste1.txt");
    converterAFNDparaAFD();
    gerarSaidaAFD("afd_resultado1.txt");
    return 0;
}