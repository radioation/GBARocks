#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_

#include "GBA_comm.h"

typedef struct {
    char path[128];
    char query[128];
    char body[256];
} http_request_state;


#define MAX_HTTP_BUFFER 4096

void http_set_server_name( char* server );
void http_set_path( char* path );
void http_set_query( char* query );
void http_set_body( char* body );
void http_reset_request( );

void http_get_request( cr_response* response );
void http_post_request( cr_response* response );



#endif // _HTTP_CLIENT_H_
