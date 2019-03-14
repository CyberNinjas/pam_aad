PAMDIR = "${PREFIX}/lib/security"

all: dlopen 

dlopentest:
	${CC} ${CFLAGS} ${LDFLAGS} -fPIE -fstack-protector -Wall -o $@ $@.c -ldl

dlopen: dlopentest
	./dlopentest "${PAMDIR}/pam_aad.so"
