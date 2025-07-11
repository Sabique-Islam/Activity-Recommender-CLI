#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include </opt/homebrew/opt/libmicrohttpd/include/microhttpd.h>

struct MHD_Daemon* http_server_init(int port);
enum MHD_Result handle_request(void *cls,
                             struct MHD_Connection *connection,
                             const char *url,
                             const char *method,
                             const char *version,
                             const char *upload_data,
                             size_t *upload_data_size,
                             void **con_cls);
void http_server_destroy(struct MHD_Daemon *daemon);

#endif
