#ifndef API_H
#define API_H

#include </opt/homebrew/opt/libmicrohttpd/include/microhttpd.h>

enum MHD_Result handle_recommend_endpoint(struct MHD_Connection *connection,
                                        const char *upload_data,
                                        size_t *upload_data_size,
                                        void **con_cls);

#endif
