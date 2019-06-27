FROM cyberninjas/pam_aad:ubuntu

RUN unset VERSION DEBVER && \
    apt update && apt install -y \
        gdb \
        openssh-server \
        pamtester \
        strace \
        syslog-ng \
        vim

ENV PAMDIR "/lib/x86_64-linux-gnu/security"
WORKDIR /usr/src/pam_aad
CMD ["make", "-eC", "test"]
