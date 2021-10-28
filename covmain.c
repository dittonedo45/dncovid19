#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int sort_by = NULL;
typedef int ( covid_main_t ) ( signed, char ** );

struct myOPTS {
 char *program;
 covid_main_t *main;
};

covid_main_t summary_main, nation_main;

int main_help ( struct myOPTS *MAINoptions )
{
 fprintf ( stderr, "covid19 [Sub] <args>\n" );
 fprintf ( stderr, "SubPograms:\n" );
 struct myOPTS *p = MAINoptions;
 while( p ) {
  if( !p->program )
   break;
  if( p->main && p->program ) {
   fprintf ( stderr, "\t%19s at main (%p)\n", p->program, p->main );
   fprintf ( stderr, "\t eg %% covid19 %s <Args> \n", p->program, p->main );
  }
  p++;
 }
}

int main ( signed Argsc, char *( Args[] ) )
{
 struct myOPTS ( MAINoptions[] ) = {
  {"summary", summary_main},
  {"international", summary_main},

  {"national", nation_main},
  {"country", nation_main},

  {NULL, NULL}

 };
 struct myOPTS *p = MAINoptions;
 if( !Args[1] ) {
  main_help ( MAINoptions );
  return 3;
 }

 while( p ) {
  if( !p->program )
   break;
  if( 0 == strncmp ( p->program, Args[1], strlen ( Args[1] ) ) ) {
   return p->main ( Argsc - 1, Args + 1 );
  }
  p++;
 }
 main_help ( MAINoptions );
 return 1;
}
