EXTRA_DIST = README

noinst_PROGRAMS = \
	unit-test-server \
	unit-test-client \
	connect_ATI_FT \
	version

common_ldflags = \
	$(top_builddir)/src/libmodbus.la


unit_test_server_SOURCES = unit-test-server.c unit-test.h
unit_test_server_LDADD = $(common_ldflags)

unit_test_client_SOURCES = unit-test-client.c unit-test.h
unit_test_client_LDADD = $(common_ldflags)

version_SOURCES = version.c
version_LDADD = $(common_ldflags)

connect_ATI_FT_SOURCES = connect_ATI_FT.c connect_ATI_FT.h
connect_ATI_FT_LDADD = $(common_ldflags)

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_srcdir)/src
CLEANFILES = *~
