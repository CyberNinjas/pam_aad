FROM debian:9.7

RUN apt update && apt install -y \
        automake \
        build-essential \
        curl \
        debhelper \
        devscripts \
        git \
        indent \
        libcurl4-openssl-dev \
        libjansson-dev \
        libpam0g-dev \
        libssl-dev \
        libtool \
        pkg-config \
        quilt \
        uuid-dev

WORKDIR /tmp
RUN git clone https://github.com/benmcollins/libjwt && \
    cd libjwt && git checkout tags/v1.10.1 && \
    autoreconf -i && ./configure && make && make install

WORKDIR /tmp
RUN curl -Lo sds_2.0.0.orig.tar.gz \
    https://gitlab.com/oxr463/sds/-/archive/debian-2.0.0-1/sds-debian-2.0.0-1.tar.gz \
    && tar -xf sds_2.0.0.orig.tar.gz && \
    mv sds-debian-2.0.0-1 sds-2.0.0 && \
    cd sds-2.0.0 && debuild -us -uc && \
    dpkg -i ../libsds2.0.0_2.0.0-1_amd64.deb && \
    dpkg -i ../libsds-dev_2.0.0-1_amd64.deb

WORKDIR /usr/src/pam_aad
COPY . /usr/src/pam_aad

ENV PAMDIR "/lib/x86_64-linux-gnu/security"
RUN ./bootstrap.sh && \
    ./configure --with-pam-dir="${PAMDIR}" && \
    make && make install
