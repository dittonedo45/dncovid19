/*XXX This Document was modified on 1635405885 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libavformat/avio.h>
#include <jv.h>

static void cv_str_add ( jv * a, char *b, int sz )
{
 *a = jv_string_append_buf ( *a, b, sz );
}

jv cvv_download ( AVIOContext * ctx )
{
 if( !ctx )
  return jv_invalid (  );

 char buf[1054];

 int sz = 0;
 // Integer because avio_read return negative numbers in case of Errors or Notifications.

 jv ans = jv_string ( "" );

 while( 1 ) {
  sz = avio_read ( ctx, buf, sizeof ( buf ) );
  if( sz == AVERROR_EOF || sz < 0 ) {
   // NOTE: if sz is not AVERROR_EOF then there is an error, Obviously The Json is Botched
   break;
  }
  cv_str_add ( &ans, buf, sz );
 }

 return jv_parse ( jv_string_value ( ans ) );
}

jv cv_download ( char *path )
{
 AVIOContext *url = 0;
 int ret = 0;

 ret = avio_open ( &url, path, AVIO_FLAG_READ );

 if( ret < 0 )
  return jv_invalid (  );

 jv dump = cvv_download ( url );
 avio_close ( url );

 return dump;
}
