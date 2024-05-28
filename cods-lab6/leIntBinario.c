#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if(argc < 3) {
        fprintf(stderr, "Digite: %s <arquivo entrada> <arquivo saída>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "rb");
    if(!entrada) {
        fprintf(stderr, "Erro de abertura do arquivo de entrada\n");
        return 2;
    }

    FILE *saida = fopen(argv[2], "w");
    if(!saida) {
        fprintf(stderr, "Erro de abertura do arquivo de saída\n");
        fclose(entrada);
        return 3;
    }

    int numero;
    while(fread(&numero, sizeof(int), 1, entrada) == 1) {
        fprintf(saida, "%d\n", numero);
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}