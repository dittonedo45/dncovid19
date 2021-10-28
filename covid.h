
#ifndef COVID_19
#define COVID_19
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jq.h>
#include <getopt.h>
#include <jv.h>

typedef
    struct covid19_country_case {
	    jv object;
 char *Country, *CountryCode, *Date, *ID, *Slug;

 double
  NewConfirmed,
     NewDeaths, NewRecovered, TotalConfirmed, TotalDeaths, TotalRecovered;

 struct covid19_country_case *left, *right;
} COVID19;

struct COVID19_day {
 char *ID;
 char *Country;
 char *CountryCode;

 char *Lat;
 char *Lon;

 double
  Confirmed, Deaths, Recovered, Active;

 char *Date;

 struct COVID19_day *left, *right;
};

typedef
    struct info {
 char *info;
 char *data;
 int kind;
} info_t;

typedef
    enum CompTYPE {
 CT_COUNTRY,
 CT_COUNTRY_CODE,
 CT_DATE,
 CT_ID,
 CT_NEWCONFIRMED,
 CT_NEWDEATHS,
 CT_NEWRECOVERED,
 CT_SLUG,
 CT_TOTALCONFIRMED,
 CT_TOTALDEATHS,
 CT_TOTALRECOVERED,
} CompTYPE;

extern char *sort_by;
int asprintf(char **, const char * , ...);

#endif 
