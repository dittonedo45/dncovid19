#include <covid.h>

static double double_obj_get ( jv obj, char *string )
{
 return
     jv_number_value ( jv_copy
                       ( jv_object_get
                         ( jv_copy ( obj ), jv_string ( string ) ) ) );
}

static char *string_obj_get ( jv obj, char *string )
{
 return
     strdup ( jv_string_value ( jv_copy
                                ( jv_object_get
                                  ( jv_copy ( obj ),
                                    jv_string ( string ) ) ) ) );
}

int myCOMP ( a, b )
struct COVID19_day *a, *b;
{
 return a->Recovered < b->Recovered;
}

static void insert_details ( struct COVID19_day **node, jv obj )
{
 struct COVID19_day **p = node;
 struct COVID19_day pkey = {
  .ID = string_obj_get ( obj, "ID" ),
  .Country = string_obj_get ( obj, "Country" ),
  .CountryCode = string_obj_get ( obj, "CountryCode" ),
  .Lat = string_obj_get ( obj, "Lat" ),
  .Lon = string_obj_get ( obj, "Lon" ),
  .Date = string_obj_get ( obj, "Date" ),

  .Confirmed = double_obj_get ( obj, "Confirmed" ),
  .Deaths = double_obj_get ( obj, "Deaths" ),
  .Recovered = double_obj_get ( obj, "Recovered" ),
  .Active = double_obj_get ( obj, "Active" ),
 };

 while( 1 ) {
#define root (*p)
  if( !root ) {
   root = malloc ( sizeof ( *root ) );
   memcpy ( root, &pkey, sizeof ( *root ) );
   root->left = NULL;
   root->right = NULL;
   break;
  } else if( myCOMP ( &pkey, root ) ) {
   p = &root->left;
  } else {
   p = &root->right;
  }
 }
#undef root
}

static int country_main ( char *path, struct COVID19_day **root )
{
 if( !path )
  return -1;
 jv obj = jv_load_file ( path, 0 );

 if( !jv_is_valid ( obj ) )
  return -2;

 int i = 0;
 jv arr = jv_copy ( jv_array_get ( obj, 0 ) );

 jv_array_foreach ( arr, i, element ) {
  insert_details ( root, jv_copy ( element ) );
 }
}

static void Show_statistics ( struct COVID19_day *root,
                              void ( *func ) ( const struct COVID19_day *
                                               data ), int invert )
{
 if( !root )
  return;
 Show_statistics ( invert ? root->left : root->right, func, invert );
 {
  func ( ( const struct COVID19_day * )root );
 }
 Show_statistics ( !invert ? root->left : root->right, func, invert );
}

#include <time.h>
int myATOI ( char *str )
{
 if( !str )
  return 0;
 char *p = str;
 while( p ) {
  if( *p != '0' )
   break;
  p++;
 }
 return ( int )strtod ( p, NULL );
}

static time_t date_of ( char *Date )
{
 char *date = NULL;
 asprintf ( &date, "%.*s", strrchr ( Date, 'T' ) - ( Date ), ( Date ) );

 char *p = date;
 int j = 0;
 struct tm hold;

 memset ( &hold, -1, sizeof ( hold ) );

 while( p ) {
  char *tmp = p;
  char *ans = NULL;

  p = strchr ( p, '-' );
  if( p ) {
   asprintf ( &ans, "%.*s", p - tmp, tmp );
   p++;
  } else {
   asprintf ( &ans, "%s", tmp );
  }
#define y(x) (hold.tm_##x = myATOI ( ans ))
  switch ( j ) {
  case 0:
   // year 
   y ( year );
   hold.tm_year -= 1900;
   break;
  case 1:
   // month 
   y ( mon );
   break;
  case 2:
   // date
   y ( mday );
   break;
  }
#undef y
  free ( ans );
  j++;
 }
 free ( date );

 time_t sec = 0u;

 {
  char Date[1054];
  long int tt = mktime ( &hold );
  {
   tt /= ( 24 * 60 * 60 );
   tt += 4;
   tt %= 7;
  }
  hold.tm_wday = tt;
  sec = mktime ( &hold );
 }
 return sec;
}

static double percentage ( a, b )
double a, b;
{
 if( b == 0 )
  return 100.0;
 if( a == 0 )
  return 100 / b;
 return ( double )( ( a * 100 ) / b );
}

int len = 0;

double num = 0;
double total = 0;
static void SHOWcases ( const struct COVID19_day *info )
{
 char date[1054];
#define i(x) (info->x)
 if( i ( Confirmed ) == 0.0 )
  return;
 {
  time_t myDATE = date_of ( i ( Date ) );
  struct tm *hold = localtime ( &myDATE );
  strftime ( date, 1054, "%a %d %b %y", hold );
 }
 len = 22;
 num++;
 total += percentage ( info->Recovered, info->Confirmed );
 fprintf ( stderr, "|%-22s | %9.lf | %9.lf | %*.lf %%|\n", date,
           info->Confirmed, info->Recovered, sizeof ( "Recovery Percentage" ),
           percentage ( info->Recovered, info->Confirmed ) );
}

void nation_help (  )
{
 fprintf ( stderr, "... nation <args>\n" );
 fprintf ( stderr, "Where <args> are:\n" );
 char *( args[] ) = {
  "sort",
  "path",
  "help",
  NULL
 };

 char **p = args;

 while( p ) {
  if( !*p )
   break;
  fprintf ( stderr, "--%s <arg>\n", *p );
  p++;
 }
}

int nation_main ( signed Argsc, char *( Args[] ) )
{
 struct COVID19_day *root = NULL;
 char *path = NULL;

 struct option ( myOPTS[] ) = {
  {"path", 1, NULL, 'p'},
  {"sort", 1, NULL, 's'},
  {"help", 0, NULL, 'h'},
  {NULL, -1, NULL, 0}
 };

 while( 1 ) {
  int c = getopt_long ( Argsc, Args, "p:s:h", myOPTS, NULL );
  if( c == -1 )
   break;

  switch ( c ) {
  case 'p':
   path = strdup ( optarg );
   break;
  case 's':
   sort_by = strdup ( optarg );
   break;
  case 'h':
 defalut:
   nation_help (  );
   return 3;
   break;
  }
 }

 if( path ) {
  country_main ( path, &root );
  Show_statistics ( root, SHOWcases, 0 );
  fprintf ( stderr, "|%-*s | %9s | %9s | %s\n", len, "Date", "Confimed",
            "Recovered", "Recovery Percentage" );
  fprintf ( stderr, "Average Recovery Rate = %lf %%\n", total / num );
 } else
  nation_help (  );
 return 0;
}
