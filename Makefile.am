pamdir = $(libdir)/security
ACLOCAL_AMFLAGS = -I m4
dist_doc_data = README.md

MODULES_LDFLAGS = -PIC -fno-stack-protector -avoid-version -module -shared -export-dynamic

lib_LTLIBRARIES= pam-aad.la

pam_aad_la_SOURCES = src/aad.c

CORE_SRC = 
CORE_SRC += src/rest.c src/rest.h
CORE_SRC += src/cJSON.c src/cJSON.h
CORE_SRC += src/utils.c src/utils.h
CORE_SRC += src/jwt.c src/jwt.h

pam_aad_la_SOURCES  += $(CORE_SRC)

pam_aad_la_LIBADD = -lpam -lssl -lcrypto -lm -ljwt
pam_aad_la_CFLAGS = $(AM_CFLAGS)
pam_aad_la_LDFLAGS = $(AM_LDFLAGS) $(MODULES_LDFLAGS) -export-symbols-regex "pam_sm_(setcred|open_session|authenticate)"

super-clean: maintainer-clean
	rm -fr aclocal autom4te.cache/ m4 missing libtool config.guess
	rm -fr config.lt config.status config.sub configure depcomp
	rm -fr libtool install-sh *~ Makefile aclocal.m4 config.h.in ltmain.sh
	rm -fr Makefile.in
