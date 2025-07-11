#include "../include/http_server.h"
#include "../include/api.h"
#include "../include/utils.h"
#include "../include/json_handler.h"
#include <string.h>
#include <stdlib.h>

struct MHD_Daemon* http_server_init(int port) {
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_AUTO | MHD_USE_INTERNAL_POLLING_THREAD,
                            port,
                            NULL,
                            NULL,
                            &handle_request,
                            NULL,
                            MHD_OPTION_END);
    return daemon;
}

static enum MHD_Result send_response(struct MHD_Connection *connection,
                                   const char *response_str,
                                   unsigned int status_code) {
    struct MHD_Response *response;
    enum MHD_Result ret;

    response = MHD_create_response_from_buffer(strlen(response_str),
                                             (void*)response_str,
                                             MHD_RESPMEM_MUST_COPY);
    
    MHD_add_response_header(response, "Content-Type", "application/json");
    MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(response, "Access-Control-Allow-Methods", "POST, OPTIONS");
    MHD_add_response_header(response, "Access-Control-Allow-Headers", "Content-Type");
    
    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);

    return ret;
}

enum MHD_Result handle_request(void *cls,
                             struct MHD_Connection *connection,
                             const char *url,
                             const char *method,
                             const char *version,
                             const char *upload_data,
                             size_t *upload_data_size,
                             void **con_cls) {
    (void)cls;
    (void)version;

    if (strcmp(method, "OPTIONS") == 0) {
        return send_response(connection, "", MHD_HTTP_OK);
    }

    if (strcmp(method, "POST") != 0) {
        const char *error = create_error_response("METHOD_NOT_ALLOWED", "Only POST method is allowed");
        enum MHD_Result ret = send_response(connection, error, MHD_HTTP_METHOD_NOT_ALLOWED);
        free((void*)error);
        return ret;
    }

    if (strcmp(url, "/recommend") == 0) {
        return handle_recommend_endpoint(connection, upload_data, upload_data_size, con_cls);
    }

    const char *error = create_error_response("NOT_FOUND", "Endpoint not found");
    enum MHD_Result ret = send_response(connection, error, MHD_HTTP_NOT_FOUND);
    free((void*)error);
    return ret;
}

void http_server_destroy(struct MHD_Daemon *daemon) {
    if (daemon) {
        MHD_stop_daemon(daemon);
    }
}
