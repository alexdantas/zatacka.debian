
rm=/bin/rm -f

INCLUDES=-I$(top_srcdir)

zatacka_LDFLAGS = -L$(top_builddir)/src

BUILT_SOURCES = defaults.h
DISTCLEANFILES = defaults.h
CONFIG_CLEAN_FILES = defaults.h
defaults.h: Makefile
	echo '//This file is generated from makefile. Do not edit!' > defaults.h
	echo '#define DEFAULT_LIBDIR "@DEFAULT_LIBDIR@"' >> defaults.h
	echo '#define PACKAGE "@PACKAGE@"' >> defaults.h

zatacka.$(OBJEXT): defaults.h


bin_PROGRAMS= zatacka

.c.o:
	$(rm) $@
	$(CXX) $(CXXFLAGS) -c $*.c


zatacka_SOURCES= main.cpp \
                fx.cpp  fx.h \
		SDL_prim.c SDL_prim.h

uninstall-local:
	for i in $(bin_PROGRAMS); do \
		rm -f $(exec_prefix)/bin/$$i; \
	done

