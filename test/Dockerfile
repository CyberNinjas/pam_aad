FROM cyberninjas/pam_aad:latest

RUN apt update && apt install -y \
        gdb \
        openssh-server \
        pamtester \
        strace \
        syslog-ng \
        vim

WORKDIR /usr/src/pam_aad
CMD ["make", "-eC", "test"]
