/*
PARCIAL 2 SISTEMAS OPERATIVOS
PROFESOR: JUAN FELIPE MUÑOZ FERNADEZ

ESTUDIANTES:
MÓNICA SOFÍA RESTREPO LEÓN (morestrepol@unal.edu.co) CC 1000181289
LUIS ALEJANDRO VARELA OJEDA (luvarelao@unal.edu.co) CC 1017280316

NOTA: esta código fue corrido y realizado en replit

gcc ParcialMoniLuis2.c -o ParcialMoniLuis2
./ParcialMoniLuis2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Contadores globales
int contador_TLB = 0;
int mas_antiguo = 0;
// Variable para apuntar siempre al TLB
char *tlb_original; 

void decimalToBinary(int num, int* binary, int size) {
    for (int i = size - 1; i >= 0; i--) {
        binary[i] = num & 1;
        num >>= 1;
    }
}

int binaryToDecimal(const int* binary, int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result = result * 2 + binary[i];
    }
    return result;
}

int verficarTLB(long n) {
    char* current_ptr = tlb_original;
    long dir_memoria = n;

    //Buscamos la direccion en el TLB 
    for (int step = 0; step < contador_TLB; step++) {
        long *direccion_ptr = (long*)current_ptr;
        if (*direccion_ptr == n) {return 1;}  // 
        current_ptr += 8;
    }
    // Sino está y aun queda esapcio en la pila
      if (contador_TLB<5){
        char* current_ptr = tlb_original;
        current_ptr += (8*contador_TLB);
        memcpy(current_ptr, &dir_memoria , sizeof(long));
        contador_TLB++;
      }
       //sino queda espacio en la pila sobreescribimos en el mas antiguo 
      else{
        char* current_ptr = tlb_original;
        current_ptr += (8* mas_antiguo);
        if (mas_antiguo>4){mas_antiguo = 0;}
        memcpy(current_ptr, &dir_memoria , sizeof(long));
        mas_antiguo++;
      }

    return 0;
}

int main(int argc, char* argv[]) {

    //inicamos el TLB con un espacio de 285 bytes
    tlb_original = (char*)malloc(285);

    while (1) {
        long direccion; //
        char entrada_consola[32];
        printf("Ingrese dirección virtual: ");
        scanf("%s", entrada_consola);
        if (strcmp(entrada_consola, "s") == 0) {
            printf("Good bye!\n");
            free(tlb_original);
            exit(1);
        }
        struct timespec start, end;
        clock_gettime(CLOCK_REALTIME, &start);

        direccion = atol(entrada_consola);
        int pagina = direccion / 4096;   // 4096 byte son 4Kib que miden las paginas
        int desplazamiento = direccion % 4096;    //el desplazamiento es lo que sobre
        int pagina_binario[20],aux, desplazamiento_binario[12];
        decimalToBinary(pagina, pagina_binario, 20);
        decimalToBinary(desplazamiento, desplazamiento_binario, 12);      

        printf("Direccion: %ld\n", direccion);
        if (verficarTLB(direccion) == 1) { printf("TLB Hit\n"); }
        else {for(long long i=0;i<1e5;i++){aux*=aux;aux%=10;}
          printf("TLB Miss\n"); }

        printf("Pagina: %d\n", pagina);
        printf("Desplazamiento: %d\n", desplazamiento);
        printf("Pagina en binario: ");
        for (int i = 0; i < 20; i++) {
            printf("%d", pagina_binario[i]);
        }
        printf("\nDesplazamiento en binario: ");
        for (int i = 0; i < 12; i++) {
            printf("%d", desplazamiento_binario[i]);
        }

        clock_gettime(CLOCK_REALTIME, &end);
        double elapsedTime = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("\nTiempo: %f segundos\n", elapsedTime);

    }
    free(tlb_original);
    return 0;
}
