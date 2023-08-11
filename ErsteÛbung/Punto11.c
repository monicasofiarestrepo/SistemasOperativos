#include <stdio.h>

int main(void) {
  signed short int ssint = 10;
  long longint = 10;
  size_t tssint = sizeof(ssint);
  size_t tlint = sizeof(longint);
  printf("tssint = %zd\n", tssint);
  printf("tlint = %zd\n", tlint);
  return 0;
 //sizeof imprime el tamaño en bytes de las variables. 

}