#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int lifeline = 4;

int set_bit(int* val, int bit_position)
 {
   *val = *val | (1 << bit_position);
 }

 int clear_bit(int val, int bit_position)
  {
    return val & ~(1 << bit_position);
  }

  int toggle_bit(int val, int bit_position)
   {
     return val ^ (1 << bit_position);
   }

   int isSet(int val, int bit_position) {
       return (val & (1 << bit_position));
   }

int main(int argc, char const *argv[]) {
    printf("%d\n", isSet(1, 2));
}
