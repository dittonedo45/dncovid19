#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int sort_by = NULL;
int summary_main ( signed Argsc, char *( Args[] ) );
int nation_main ( signed Argsc, char *( Args[] ) );

struct myOPTS {
 char *program;
 int ( *main ) ( int, char *( [] ) );
} ( MAINoptions[] ) = {
 {"summary", summary_main},
 {"international", summary_main},

 {"national", nation_main},
 {"country", nation_main},

 {NULL, NULL}

};

int main_help (  )
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
 struct myOPTS *p = MAINoptions;
 if( !Args[1] ) {
  main_help (  );
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
 main_help (  );
 return 1;
}
