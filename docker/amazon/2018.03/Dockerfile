FROM amazonlinux:2018.03

RUN yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-6.noarch.rpm \
    && yum update -y && yum group install -y 'Development Tools' \
    && yum install -y \
        git \
        indent \
        libcurl-devel \
        jansson-devel \
        pam-devel \
        openssl-devel \
        libuuid-devel

WORKDIR /tmp
RUN git clone https://github.com/benmcollins/libjwt && \
    cd libjwt && git checkout tags/v1.10.1 && \
    autoreconf -i && ./configure && make && make install

# See: https://github.com/antirez/sds/issues/97
WORKDIR /tmp
RUN git clone https://github.com/antirez/sds libsds && \
    cd libsds && git checkout tags/2.0.0 && \
    echo "typedef int sdsvoid;" >> sdsalloc.h && \
    gcc -fPIC -fstack-protector -std=c99 -pedantic -Wall \
        -Werror -shared -o libsds.so.2.0.0 -Wl,-soname=libsds.so.2.0.0 \
        sds.c sds.h sdsalloc.h && \
    cp -a libsds.so.2.0.0 /usr/local/lib/ && \
    ln -s /usr/local/lib/libsds.so.2.0.0 /usr/local/lib/libsds.so && \
    ln -s /usr/local/lib/libsds.so.2.0.0 /usr/local/lib/libsds.so.2 && \
    ldconfig && mkdir -p /usr/local/include/sds && \
    cp -a sds.h /usr/local/include/sds/ 

WORKDIR /usr/src/pam_aad
COPY . /usr/src/pam_aad

ENV PAMDIR="/lib64/security" \
    LD_LIBRARY_PATH="${LD_LIBRARY_PATH}:/usr/local/lib:/usr/local/lib64"
RUN ./bootstrap.sh && \
    ./configure --with-pam-dir="${PAMDIR}" && \
    make && make install
