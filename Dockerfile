FROM docker.io/library/ubuntu:22.04

RUN apt-get update && \
    apt-get install -y libssl-dev libssl3 make gcc && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /docodile

COPY ./src/ ./src/
COPY ./Makefile ./

RUN make build

EXPOSE 6006

CMD ["./src/build/docodile", "serve"]