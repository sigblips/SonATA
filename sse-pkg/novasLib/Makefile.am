
# NOVAS - US Naval Observatory Astronomy Library
# http://aa.usno.navy.mil/software/novas/novas_info.php

PUBLIC_HEADER_FILES = \
	novas.h \
	novascon.h \
	solarsystem.h

lib_LIBRARIES = libnovas.a

libnovas_a_SOURCES = \
	$(PUBLIC_HEADER_FILES) \
	novas.c \
	novascon.c \
	readeph0.c \
	solsys3.c

# public headers to include in 'make install' target
include_HEADERS = $(PUBLIC_HEADER_FILES)

check_PROGRAMS = checkout

TESTS = run_checkout

checkout_SOURCES = \
	checkout-st.c \
	$(libnovas_a_SOURCES)

checkout_LDADD = -lm

# Force the test datafiles to be included in the distribution
EXTRA_DIST = \
	run_checkout \
	checkout-st.no
