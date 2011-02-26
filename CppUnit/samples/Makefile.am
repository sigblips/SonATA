SUBDIRS = Multicaster

noinst_PROGRAMS = TestRunnerExample

INCLUDES = -I$(srcdir)/../test/framework -I$(srcdir)/../test/textui \
		-I$(srcdir)/Multicaster

TestRunnerExample_LDADD = ../test/textui/libcutextui.a \
			../test/framework/libcu.a \
			Multicaster/libmulticastertest.a \
			Multicaster/libmulticaster.a

TestRunnerExample_SOURCES =    TestTest.h \
			ExampleTestCase.cpp \
			ExampleTestCase.h \
			TestRunnerExample.cpp

#EXTRA_DIST = TestRunner.mak

TESTS = runtests

EXTRA_DIST = runtests
