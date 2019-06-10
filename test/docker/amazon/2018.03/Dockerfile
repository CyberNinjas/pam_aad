FROM cyberninjas/pam_aad:amazon-2018.03

RUN yum update -y && yum install -y \
        gdb \
        openssh-server \
        pamtester \
        strace \
        rsyslog \
        vim

ENV PAMDIR '/lib64/security'
WORKDIR /usr/src/pam_aad
CMD PAMDIR='/lib64/security' make -eC test
