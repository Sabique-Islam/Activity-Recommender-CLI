FROM gcc:latest
WORKDIR /app

RUN apt-get update && \
    apt-get install -y make && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

COPY . .

RUN make clean && make

RUN mkdir -p /app/data

CMD ["./arc"]