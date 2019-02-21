FROM debian:9.7

RUN apt update && apt install -y \
        automake \
        build-essential \
        cmake \
        git \
        libjansson-dev \
        libpam0g-dev \
        libssl-dev \
        libtool \
        pkg-config

WORKDIR /tmp
RUN git clone https://github.com/benmcollins/libjwt && \
    cd libjwt && git checkout tags/v1.10.1 && \
    autoreconf -i && ./configure && make && make install

WORKDIR /tmp
RUN git clone https://github.com/DaveGamble/cJSON && \
    cd cJSON && git checkout tags/v1.7.10 && \
    mkdir build && cd build && cmake .. && make && make install

WORKDIR /usr/src/pam_aad
COPY . /usr/src/pam_aad

RUN ./bootstrap.sh && ./configure && make
