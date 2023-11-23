#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Contadores globales
int fifo = 0;
int tlbCount = 0;

// Variables para el TLB
char* tlb_ptr;  // Puntero al bloque de memoria contigua para el TLB

#define TLB_SIZE 258  // Tamaño del TLB

void decimalToBinary(int num, int* binary, int size) {
    int i;
    for (i = size - 1; i >= 0; i--) {
        binary[i] = num & 1;
        num >>= 1;
    }
}

int binaryToDecimal(const int* binary, int size) {
    int result = 0;
    int i;
    for (i = 0; i < size; i++) {
        result = result * 2 + binary[i];
    }
    return result;
}

void printTime(double time_spent) {
    printf("Tiempo: %f segundos\n", time_spent);
}

void initializeTLB() {
    // Inicializar el TLB en el segmento heap
    tlb_ptr = (char*)malloc(TLB_SIZE * sizeof(char));

    if (tlb_ptr == NULL) {
        // Manejar error de asignación de memoria
        fprintf(stderr, "Error al asignar memoria para el TLB\n");
        exit(1);
    }
}

void freeTLB() {
    // Liberar memoria del TLB al final del programa
    free(tlb_ptr);
}

int checkTLB(int n) {
    char* current_ptr = tlb_ptr;

    for (int i = 0; i < TLB_SIZE; i++) {
        int* direccion_ptr = (int*)current_ptr;

        if (*direccion_ptr == n) {
            return 1;  // TLB Hit
        }

        current_ptr += sizeof(int);
    }

    // TLB Miss
    for (int i = TLB_SIZE - 1; i > 0; i--) {
        char* current_ptr = tlb_ptr + i * sizeof(int);
        char* previous_ptr = tlb_ptr + (i - 1) * sizeof(int);

        *(int*)current_ptr = *(int*)previous_ptr;
    }

    *(int*)tlb_ptr = n;

    tlbCount++;
    return 0;
}

int main(int argc, char* argv[]) {
    initializeTLB();

    while (1) {
        int n;
        printf("Ingrese dirección virtual: ");
        if (scanf("%d", &n) != 1) {
            printf("Good bye!\n");
            freeTLB();
            exit(1);
        }

        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);

        int pag = n >> 12;        // Desplazamiento de 12 bits para obtener la página
        int despl = n & 0xFFF;    // Máscara de 12 bits para obtener el desplazamiento

        printf("Direccion: %d\n", n);
        if (checkTLB(n) == 1) {
            printf("TLB Hit\n");
        } else {
            fifo++;
            printf("TLB Miss\n");
        }

        printf("Pagina: %d\n", pag);
        printf("Desplazamiento: %d\n", despl);

        int pagina_binario[20], desplazamiento_binario[12];
        decimalToBinary(pag, pagina_binario, 20);
        decimalToBinary(despl, desplazamiento_binario, 12);

        printf("Pagina en binario: ");
        for (int i = 0; i < 20; i++) {
            printf("%d", pagina_binario[i]);
        }
        printf("\nDesplazamiento en binario: ");
        for (int i = 0; i < 12; i++) {
            printf("%d", desplazamiento_binario[i]);
        }

        printf("\n");

        clock_gettime(CLOCK_REALTIME, &end);
        double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printTime(elapsedTime);

        printf("\n");
    }

    freeTLB();
    return 0;
}
