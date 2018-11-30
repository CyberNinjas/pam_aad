FROM ubuntu:18.04

WORKDIR /usr/src/cyberninjas/pam_aad
COPY . /usr/src/cyberninjas/pam_aad

RUN apt update && apt upgrade && \
    apt install -y automake autolibjwt-dev build-essential libpam0g-dev libssl-dev libtool

RUN autoreconf --install && \
    ./configure && \
    make

CMD ["make", "check"]
