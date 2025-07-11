FROM gcc:latest
WORKDIR /app
COPY . .
RUN apt-get update --yes && apt-get install --yes \
    libmicrohttpd-dev \
    && rm -rf /var/lib/apt/lists/*
RUN sed -i 's|#include </opt/homebrew/opt/libmicrohttpd/include/microhttpd.h>|#include <microhttpd.h>|g' include/http_server.h src/api.c include/api.h
RUN make clean && make
EXPOSE 6969
CMD ["./arc-server"]