FROM ubuntu:18.04

RUN apt update && apt install -y \
        software-properties-common && \
    add-apt-repository -y ppa:lramage/sds

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
        libjwt-dev \
        libpam0g-dev \
        libsds-dev \
        libssl-dev \
        libtool \
        pkg-config \
        quilt \
        uuid-dev

WORKDIR /usr/src/pam_aad
COPY . /usr/src/pam_aad
ENV PAMAADVERSION="0.0.3" PAMAADDEBVERSION="0.0.3-1"
RUN tar cvzf "../pam-aad_${PAMAADVERSION}.orig.tar.gz" --exclude='.git*' . && \
    debuild -us -uc -d -i'(.*)' && \
    dpkg -i "../libpam-aad_${PAMAADDEBVERSION}_amd64.deb"
