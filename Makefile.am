ACLOCAL_AMFLAGS = -I m4

dist_doc_data = README.md

AM_LDFLAGS = -Wl,--strip-debug -Wl,--build-id=none -no-undefined
MODULES_LDFLAGS = -fPIC -fno-stack-protector -avoid-version -module -shared -export-dynamic

libdir = $(PAMDIR)

lib_LTLIBRARIES= pam_aad.la

pam_aad_la_SOURCES = pam_aad.c
pam_aad_la_LIBADD = -lcurl -ljansson -ljwt -lpam -lsds -luuid
pam_aad_la_CFLAGS = $(AM_CFLAGS)
pam_aad_la_LDFLAGS = $(AM_LDFLAGS) $(MODULES_LDFLAGS) -export-symbols-regex "^pam_sm_"

INDENT = indent

reformat:
	@VERSION_CONTROL=none $(INDENT) $(pam_aad_la_SOURCES)

clean-all: maintainer-clean
	rm -fr aclocal autom4te.cache/ m4 missing libtool config.guess
	rm -fr config.lt config.status config.sub configure depcomp
	rm -fr libtool install-sh *~ Makefile aclocal.m4 config.h.in ltmain.sh
	rm -fr Makefile.in

install-exec-hook:
	rm -f $(DESTDIR)$(libdir)/pam_aad.la
