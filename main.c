#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_LINHAS 100
#define TAM_LINHAS 256
#define MAX_ALFABETO 50
#define TAM_ALFABETO 50 
#define MAX_ESTADOS 50
#define TAM_ESTADOS 128
#define MAX_TRANSICOES 100
#define TAM_TRANSICOES 100

typedef struct {
    char texto[MAX_LINHAS][TAM_LINHAS];
    int qtd;
} ListaDeLinhas;

typedef struct {
    char alfabeto[MAX_ALFABETO][TAM_ALFABETO];
    int qtd_alfabeto;
    char estados[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_estados;
    char estado_inicial[TAM_ESTADOS];
    char estados_finais[MAX_ESTADOS][TAM_ESTADOS];
    int qtd_finais;
    char transicoes[MAX_TRANSICOES][TAM_TRANSICOES];
    int qtd_transicoes;
} Automato;

void carregarArquivo(const char *Nomearquivo, ListaDeLinhas *lista) {
    FILE *arquivo = fopen(Nomearquivo, "r");
    lista->qtd = 0;
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo!\n");
        return;
    }
    char linha[TAM_LINHAS];
    while (fgets(linha, TAM_LINHAS, arquivo) != NULL) {
        if (linha[0] == '#' || linha[0] == '\n' || linha[0] == '\r') {
            continue;
        }
        if (lista->qtd >= MAX_LINHAS) {
            break;
        }
        linha[strcspn(linha, "\r\n")] = 0;
        strcpy(lista->texto[lista->qtd], linha);
        lista->qtd++;
    }
    fclose(arquivo);
}

void interpretarAFND(ListaDeLinhas *lista, Automato *afnd) {
    afnd->qtd_alfabeto = afnd->qtd_estados = afnd->qtd_finais = afnd->qtd_transicoes = 0;
    for (int i = 0; i < lista->qtd; i++) {
        char buffer[TAM_LINHAS];
        strcpy(buffer, lista->texto[i]);
        char *token = strtok(buffer, " ");
        if (!token) continue;

        if (strcmp(token, "A") == 0) {
            while ((token = strtok(NULL, " "))) strcpy(afnd->alfabeto[afnd->qtd_alfabeto++], token);
        } else if (strcmp(token, "Q") == 0) {
            while ((token = strtok(NULL, " "))) strcpy(afnd->estados[afnd->qtd_estados++], token);
        } else if (strcmp(token, "q") == 0) {
            token = strtok(NULL, " ");
            if (token) strcpy(afnd->estado_inicial, token);
        } else if (strcmp(token, "F") == 0) {
            while ((token = strtok(NULL, " "))) strcpy(afnd->estados_finais[afnd->qtd_finais++], token);
        } else if (strcmp(token, "T") == 0) {
            char *orig = strtok(NULL, " ");
            char *simb = strtok(NULL, " ");
            char *dest = strtok(NULL, " ");
            if (orig && simb && dest) {
                sprintf(afnd->transicoes[afnd->qtd_transicoes++], "%s %s %s", orig, simb, dest);
            }
        }
    }
}

void obterDestinos(Automato *afnd, char *conjunto, char *simbolo, char *res) {
    res[0] = '\0';
    char temp_conj[TAM_ESTADOS];
    strcpy(temp_conj, conjunto);
    char *estado_individual = strtok(temp_conj, "[,]");
    
    while (estado_individual != NULL) {
        for (int i = 0; i < afnd->qtd_transicoes; i++) {
            char t_orig[TAM_ESTADOS], t_simb[20], t_dest[TAM_ESTADOS];
            sscanf(afnd->transicoes[i], "%s %s %s", t_orig, t_simb, t_dest);
            if (strcmp(t_orig, estado_individual) == 0 && strcmp(t_simb, simbolo) == 0) {
                if (!strstr(res, t_dest)) {
                    if (strlen(res) > 0) strcat(res, ",");
                    strcat(res, t_dest);
                }
            }
        }
        estado_individual = strtok(NULL, "[,]");
    }
}

void converterAFNDparaAFD(Automato *afnd, Automato *afd) {
    afd->qtd_alfabeto = afnd->qtd_alfabeto;
    for(int i = 0; i < afnd->qtd_alfabeto; i++) {
        strcpy(afd->alfabeto[i], afnd->alfabeto[i]);
    }
    
    sprintf(afd->estados[0], "%s", afnd->estado_inicial);
    strcpy(afd->estado_inicial, afd->estados[0]);
    afd->qtd_estados = 1;
    afd->qtd_transicoes = 0;

    for (int i = 0; i < afd->qtd_estados; i++) {
        for (int j = 0; j < afd->qtd_alfabeto; j++) {
            char res_conjunto[TAM_ESTADOS] = "";
            obterDestinos(afnd, afd->estados[i], afd->alfabeto[j], res_conjunto);
            
            if (strlen(res_conjunto) > 0) {
                char novo_nome[TAM_ESTADOS];
                sprintf(novo_nome, "%s", res_conjunto);

                int indice_existente = -1;
                for (int k = 0; k < afd->qtd_estados; k++) {
                    if (strcmp(afd->estados[k], novo_nome) == 0) {
                        indice_existente = k;
                        break;
                    }
                }

                if (indice_existente == -1 && afd->qtd_estados < MAX_ESTADOS) {
                    strcpy(afd->estados[afd->qtd_estados], novo_nome);
                    indice_existente = afd->qtd_estados;
                    afd->qtd_estados++;
                }

                if (indice_existente != -1) {
                    sprintf(afd->transicoes[afd->qtd_transicoes++], "%s %s %s", 
                            afd->estados[i], afd->alfabeto[j], afd->estados[indice_existente]);
                }
            }
        }
    }

    afd->qtd_finais = 0;
    for (int i = 0; i < afd->qtd_estados; i++) {
        for (int f = 0; f < afnd->qtd_finais; f++) {
            if (strstr(afd->estados[i], afnd->estados_finais[f])) {
                strcpy(afd->estados_finais[afd->qtd_finais++], afd->estados[i]);
                break; 
            }
        }
    }
}

void saida(Automato *afd, const char *fileSaida) {
    FILE *file = fopen(fileSaida, "w");
    if (file == NULL) return;
    
    fprintf(file, "# Automato Finito Deterministico equivalente\n");
    fprintf(file, "\n# Alfabeto\n");
    fprintf(file, "A");
    for(int i=0; i < afd->qtd_alfabeto; i++) fprintf(file, " %s", afd->alfabeto[i]);

    fprintf(file, "\n# Estados\n");
    fprintf(file, "Q");
        for (int i = 0; i < afd->qtd_estados; i++) {
            fprintf(file, " [%s]", afd->estados[i]);
        }
    fprintf(file, "\n");
    // fprintf(file, "Q\n");
    // for (int i = 0; i < afd->qtd_estados; i++) fprintf(file, " %s", afd->estados[i]);

    fprintf(file, "\n# Estado Inicial\n");
    fprintf(file, "\nq [%s]\n", afd->estado_inicial);


    fprintf(file, "\n# Estado Final\n");
    fprintf(file, "F");
    for(int i=0; i < afd->qtd_finais; i++) fprintf(file, " [%s]", afd->estados_finais[i]);

    fprintf(file, "\n# Transições\n");
    fprintf(file, "\n");
    for(int i=0; i < afd->qtd_transicoes; i++) fprintf(file, "T [%s]\n", afd->transicoes[i]);

    fclose(file);
}

int main() {
    ListaDeLinhas entrada;
    Automato afnd_inst, afd_inst;
    
    carregarArquivo("e1.txt", &entrada);
    interpretarAFND(&entrada, &afnd_inst);
    converterAFNDparaAFD(&afnd_inst, &afd_inst);
    saida(&afd_inst, "saida.txt");
    
    return 0;
}
