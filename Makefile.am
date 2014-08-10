# Makefile.am for ZATACKA

SUBDIRS = src
MAINTAINERCLEANFILES = aclocal.m4 config.h.in configure Makefile.in stamp-h.in
ACLOCAL_AMFLAGS = -I .
EXTRA_DIST = \
acinclude.m4 \
COPYING \
LICENSE \
README 

install-data-local:
	 @$(PRE_INSTALL)
	 $(mkinstalldirs) $(DESTDIR)$(DEFAULT_LIBDIR)/$(PACKAGE); \
	 $(INSTALL_DATA) $(srcdir)/img/main_screen.jpg $(DESTDIR)$(DEFAULT_LIBDIR)/$(PACKAGE); \
	 $(INSTALL_DATA) $(srcdir)/img/barrier.jpg $(DESTDIR)$(DEFAULT_LIBDIR)/$(PACKAGE); \
	 $(INSTALL_DATA) $(srcdir)/maps/map1.jpg $(DESTDIR)$(DEFAULT_LIBDIR)/$(PACKAGE); \
	 $(INSTALL_DATA) $(srcdir)/fonts/font.ttf $(DESTDIR)$(DEFAULT_LIBDIR)/$(PACKAGE); \
	 $(INSTALL_DATA) $(srcdir)/README $(DESTDIR)$(DEFAULT_LIBDIR)/$(PACKAGE); 

uninstall-local:
	rm -rf $(DEFAULT_LIBDIR)/zatacka/*
	rm -rf $(DEFAULT_LIBDIR)/zatacka
distclean-local:
	rm -f *\~
