
#include <stdio.h>
#include <stdlib.h>

#undef VDRVERSNUM
#define VDRVERSNUM 10307 // to keep common.h happy

#include "i18n.h"
#include "i18n.c"

int main(int argc, char *argv[])
{
  if(argc<2) return 1;
  int num=atoi(argv[1]);
  if(num<1 || num>I18nNumLanguages) return 1;

  const tI18nPhrase *p=Phrases;
  while(*p[0]) {
    if((*p)[num-1]==0 || *((*p)[num-1])==0)
      printf("missing translation for '%s'\n",*p[0]);
    p++;
    }
}

