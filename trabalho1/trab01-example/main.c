/***************************************************************
 * Description: Main function
 * Author: Edson Borin (edson@ic.unicamp.br)
 ***************************************************************/
#include "error.h"
#include <stdlib.h>
#include <stdio.h>


int main( int argc, const char* argv[] )
{
  if (argc != 2) {
    report_error("Expecting exactly one argument...", 1, -1);
  }

  printf("* Program name: %s\n", argv[0]);
  printf("* Argument    : %s\n", argv[1]);

  return 0; // Return OK
}
