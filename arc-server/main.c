#include "include/http_server.h"
#include "include/recommender.h"
#include <signal.h>
#include <unistd.h>

static volatile int keep_running = 1;

static void handle_signal(int signum) {
    if (signum == SIGINT || signum == SIGTERM) {
        keep_running = 0;
    }
}

int main() {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);

    if (initialize_recommender() != 0) {
        return 1;
    }

    struct MHD_Daemon *daemon = http_server_init(6969);
    if (!daemon) {
        return 1;
    }
    
    while (keep_running) {
        sleep(1);
    }

    http_server_destroy(daemon);
    return 0;
}