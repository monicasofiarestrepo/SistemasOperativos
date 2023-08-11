#include <stdio.h>

int main(void) {
  int mientero = 5;
  float mifloat = 5.0;
  double midouble = 3.141516;
  long double milongd = 3.1414E10;
  char michar = 'c';
  char otrochar = 99;
  signed short int misint = -10;
  char mistring[] = "Hola";
  
  printf("Hola mundo desde C\n");
  printf("mientero = %d\n", mientero);
  printf ("mifloat = %f\n", mifloat);
  printf("midouble = %lf\n", midouble);
  printf("milongdouble = %Lf\n", milongd);
  printf("michar = %c\n",michar); //LETRA
  printf("michar = %d\n",michar); //NUMERO 
  printf("otrochar = %c\n",otrochar);
  printf("misint %d\n", misint);
  printf("mistring = %s\n",mistring);
  return 0;
  }


