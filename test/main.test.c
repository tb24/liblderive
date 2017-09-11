#include <stdlib.h>
#include <stdio.h>

#include <lderive/clsys.h>

int main(int argv, char ** argc){
  
  fprintf(stdout, "argv: %d\n", argv);


  CLSYS_LSystem* lsys;
  char* fileName;
  FILE *myfile = NULL;
  char defaultFileName[] = "./test/simple.lsys";

  fileName = defaultFileName;

  if( argv > 1 )
    fileName = argc[1];



  if( !(myfile = fopen(fileName, "r")))
    return fprintf(stderr,"Couldn't find file: %s\n", fileName), -1;

  if( ! lsys_initFromFile(myfile, &lsys) )
  {
    if ( lsys == NULL )
      fprintf(stdout, "Unrecoverable error!\n");
    else {
      fprintf(stdout, "There were errors.\n");
      for(unsigned int i=1; i <= lsys_numErrors(lsys); i++)
        fprintf(stdout,"%s\n",lsys_getErrorMsg(lsys, i));

    }
    lsys_freeLSystem(lsys);
    fclose(myfile);
    exit(0);
  }
  
  fprintf(stdout, "Axiom:\n");
  lsys_fprint_lstring(stdout, lsys);
  fprintf(stdout, "Deriving...\n");
  while(lsys_deriveOnce(lsys))
  {
    fprintf(stdout, ".");
  }

  fprintf(stdout, "\nDone:\n");
  lsys_fprint_lstring(stdout, lsys);
  lsys_freeLSystem(lsys);
  fclose(myfile);

  exit(0); 

}
