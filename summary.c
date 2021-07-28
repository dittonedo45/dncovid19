#include <covid.h>

CompTYPE comp_type = CT_NEWCONFIRMED;
int invert_show = 0;

static info_t get_data ( info_t * arr, char *key )
{
 info_t *p = arr;

 while( p ) {
  if( !p->info )
   break;
  if( strcmp ( p->info, key ) == 0 )
   break;
  p++;
 }
 if( !p )
  return ( info_t ) {
  0, 0, 0};
 return *p;
}

static double mySTRTOD ( char *str )
{
 if( !str )
  return 0.0;
 return strtod ( str, NULL );
}

static COVID19 info2covid ( info_t * info )
{
 COVID19 ans = {
  .left = NULL,
  .right = NULL
 };

 memset ( &ans, 0x0, sizeof ( ans ) );
 ans.Country = get_data ( info, "Country" ).data;
 ans.CountryCode = get_data ( info, "CountryCode" ).data;
 ans.Date = get_data ( info, "Date" ).data;
 ans.ID = get_data ( info, "ID" ).data;
 ans.NewConfirmed = mySTRTOD ( get_data ( info, "NewConfirmed" ).data );
 ans.NewDeaths = mySTRTOD ( get_data ( info, "NewDeaths" ).data );
 ans.NewRecovered = mySTRTOD ( get_data ( info, "NewRecovered" ).data );
 ans.Slug = get_data ( info, "Slug" ).data;
 ans.TotalConfirmed = mySTRTOD ( get_data ( info, "TotalConfirmed" ).data );
 ans.TotalDeaths = mySTRTOD ( get_data ( info, "TotalDeaths" ).data );
 ans.TotalRecovered = mySTRTOD ( get_data ( info, "TotalRecovered" ).data );

 return ans;
}

static void insert_case ( COVID19 ** root, info_t * info,
                          int ( *myCOMP ) ( COVID19 *, COVID19 * ) )
{
 COVID19 **p = root;

 COVID19 ans = info2covid ( info );

 while( 1 ) {
  if( !*p ) {
   *p = malloc ( sizeof ( **p ) );
   memcpy ( *p, &ans, sizeof ( ans ) );
   ( *p )->left = NULL;
   ( *p )->right = NULL;
   return;
  } else if( myCOMP ( &ans, *p ) ) {
   p = &( *p )->left;
  } else {
   p = &( *p )->right;
  }
 }
}

int showdata ( jv data, COVID19 ** root,
               int ( *myCOMP ) ( COVID19 *, COVID19 * ) )
{
 int total_count = jv_array_length ( data );

 while( total_count-- > -1 ) {
  jv tmp = jv_copy ( data );
  tmp = jv_array_get ( tmp, total_count );
  if( !jv_is_valid ( tmp ) )
   break;
  {
   char *( keys[] ) = {
	"Country",
	"CountryCode",
	"Date",
	"ID",
	"NewConfirmed",
	"NewDeaths",
	"NewRecovered",
	"Premium",
	"Slug",
	"TotalConfirmed",
	"TotalDeaths",
	"TotalRecovered",
	NULL
   };
   info_t country_info[13];

   memset ( &country_info, 0x0, sizeof ( country_info ) );
   char **p = keys;
   int cc = 0;
   while( p ) {
	if( !*p )
	 break;
	jv nigga = jv_copy ( tmp );
	nigga = jv_object_get ( nigga, jv_string ( *p ) );
	switch ( jv_get_kind ( nigga ) ) {
	case JV_KIND_STRING:
	 {
	  {
	   asprintf ( &country_info[cc].info, "%s", *p );
	   asprintf ( &country_info[cc++].data, "%s", jv_string_value ( nigga ) );

	   country_info[p - keys].kind = JV_KIND_STRING;
	  }
	 }
	 break;
	case JV_KIND_NUMBER:
	 if( jv_get_kind ( nigga ) == JV_KIND_NUMBER ) {
	  {
	   asprintf ( &country_info[cc].info, "%s", *p );

	   asprintf ( &country_info[cc++].data, "%.lf", jv_number_value ( nigga ) );
	   country_info[p - keys].kind = jv_get_kind ( nigga );
	  }
	 }
	 break;
	case JV_KIND_NULL:
	case JV_KIND_OBJECT:
	case JV_KIND_INVALID:
	default:
	 break;
	};
	p++;
   }

   insert_case ( root, country_info, myCOMP );
  }
 }
}

int get_countries ( jv data, COVID19 ** root,
                    int ( *func ) ( COVID19 *, COVID19 * ) )
{

 if( !jv_is_valid ( data ) ) {
  return -1;
 }
 if( jv_get_kind ( data ) != JV_KIND_ARRAY ) {
  puts ( jv_kind_name ( jv_get_kind ( data ) ) );
  return -3;
 }
 data = jv_array_get ( data, 0 );
 data = jv_object_get ( data, jv_string ( "Countries" ) );

 showdata ( jv_copy ( data ), root, func );
}

int myCOMPwithManCases ( a, b )
COVID19 *a, *b;
{
 switch ( comp_type ) {
 case CT_COUNTRY:
  return 0 == strcmp ( a->Country, b->Country );
  break;
 case CT_COUNTRY_CODE:
  return 0 == strcmp ( a->CountryCode, b->CountryCode );
  break;
 case CT_DATE:
  return 0 == strcmp ( a->Date, b->Date );
  break;
 case CT_ID:
  return 0 == strcmp ( a->ID, b->ID );
  break;
 case CT_NEWCONFIRMED:
#define RET(x) return (a->x>b->x)
  RET ( NewConfirmed );
  break;
 case CT_NEWDEATHS:
  RET ( NewDeaths );
  break;
 case CT_NEWRECOVERED:
  RET ( NewRecovered );
  break;
 case CT_SLUG:
  return 0 == strcmp ( a->Slug, b->Slug );
  break;
 case CT_TOTALCONFIRMED:
  RET ( TotalConfirmed );
  break;
 case CT_TOTALDEATHS:
  RET ( TotalDeaths );
  break;
 case CT_TOTALRECOVERED:
 defalt:
  RET ( TotalRecovered );
  break;
 };
}

void print_case ( COVID19 * root )
{
 if( !root )
  return;
 print_case ( invert_show ? root->right : root->left );

 if( root->TotalConfirmed != 0 ) {
  double percentage = ( root->TotalRecovered * 100 ) / root->TotalConfirmed;
  fprintf ( stdout,
            "Country: %s.\nConfirmed Cases: \033[31m%d\033[0m \nConfirmed -> %.lf\nRecovered -> %.lf\n\n",
            root->Country, abs ( root->TotalConfirmed - root->TotalRecovered ),
            root->TotalConfirmed, root->TotalRecovered );
 }
 print_case ( !invert_show ? root->right : root->left );
}

void summary_help (  )
{
 fprintf ( stderr, "... summ[ary] <Arguments>\n" );
 fprintf ( stderr, "Arguments:\n" );
 {
  char *( comps[] ) = {
   "sort",
   "help",
   "invert", NULL
  };
  char **p = comps;
  while( p ) {
   if( !*p )
	break;
   fprintf ( stderr, " .. comp %s\n", *p );
   p++;
  }
 }
 fprintf ( stderr, " --comp \n" );
 {
  char *( comps[] ) = {
   "Country",
   "CountryCode",
   "Date",
   "ID",
   "NewConfirmed",
   "NewDeaths",
   "NewRecovered",
   "Premium",
   "Slug",
   "TotalConfirmed",
   "TotalDeaths",
   "TotalRecovered",
   NULL
  };
  char **p = comps;
  while( p ) {
   if( !*p )
	break;
   fprintf ( stderr, " .. comp %s\n", *p );
   p++;
  }
 }
}

int summary_main ( signed Argsc, char *( Args[] ) )
{
 COVID19 *root = NULL;

 char *path = NULL;

 struct option ( myOPTS[] ) = {
  {"path", 1, NULL, 'p'},
  {"sort", 1, NULL, 's'},
  {"help", 0, NULL, 'h'},
  {"comp", 1, NULL, 'c'},
  {"invert", 0, NULL, 'i'},
  /*
     comp yn
     invert ...
   */
  {NULL, -1, NULL, 0}
 };

 while( 1 ) {
  int c = getopt_long ( Argsc, Args, "p:s:hc:i", myOPTS, NULL );
  if( c == -1 )
   break;

  switch ( c ) {
  case 'p':
   path = strdup ( optarg );
   break;
  case 's':
   sort_by = strdup ( optarg );
   break;
  case 'i':
   invert_show = !invert_show;
   break;
  case 'c':
   {
	{
	 struct {
	  CompTYPE type;
	  char *str;
	 } *p, ( comps[] ) = {
	  {CT_COUNTRY, "Country"},
	  {CT_DATE, "Date"},
	  {CT_ID, "ID"},
	  {CT_NEWCONFIRMED, "NewConfirmed"},
	  {CT_NEWDEATHS, "NewDeaths"},
	  {CT_NEWRECOVERED, "NewRecovered"},
	  {CT_SLUG, "Slug"},
	  {CT_TOTALCONFIRMED, "TotalConfirmed"},
	  {CT_TOTALDEATHS, "TotalDeaths"},
	  {CT_TOTALRECOVERED, "TotalRecovered"},
	  {-1, NULL}
	 };
	 p = comps;
	 while( p ) {
	  if( !p->str )
	   break;
	  if( 0 == strncasecmp ( p->str, optarg, strlen ( optarg ) ) ) {
	   comp_type = p->type;
	   break;
	  }
	  p++;
	 }
	}
	if( comp_type == -1 ) {
	 nation_help (  );
	 return 4;
	}
   }
   break;
  case 'h':
  default:
   nation_help (  );
   return 3;
   break;
  }
 }

 if( path ) {
  jv jj = jv_load_file ( path, 0 );
  if( jv_is_valid ( jj ) ) {
   get_countries ( jv_copy ( jj ), &root, myCOMPwithManCases );
   print_case ( root );
   return 0;
  } else {
   fprintf ( stderr, "File Corrupted.\n" );
  }
 } else
  summary_help (  );
}
