# Makefile for hev-scgi-handler-filebox
 
CPP=cpp
CC=gcc
AR=ar
LD=ld
PKG_DEPS=glib-2.0 gio-2.0
CCFLAGS=-O3 `pkg-config --cflags $(PKG_DEPS)` -I../hev-scgi-server-library/include
LDFLAGS=
 
SRCDIR=src
BINDIR=bin
BUILDDIR=build

STATIC_TARGET=$(BINDIR)/libhev-scgi-handler-filebox.a
SHARED_TARGET=$(BINDIR)/libhev-scgi-handler-filebox.so

$(STATIC_TARGET): CCFLAGS+=-DSTATIC_MODULE
$(SHARED_TARGET): CCFLAGS+=-fPIC
$(SHARED_TARGET): LDFLAGS+=-shared `pkg-config --libs $(PKG_DEPS)` -L../hev-scgi-server-library/bin -lhev-scgi-server

CCOBJSFILE=$(BUILDDIR)/ccobjs
-include $(CCOBJSFILE)
LDOBJS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o,$(wildcard src/*.c))

DEPEND=$(LDOBJS:.o=.dep)
 
shared : $(CCOBJSFILE) $(SHARED_TARGET)
	@$(RM) $(CCOBJSFILE)

static : $(CCOBJSFILE) $(STATIC_TARGET)
	@$(RM) $(CCOBJSFILE)
 
clean : 
	@echo -n "Clean ... " && $(RM) $(BINDIR)/* $(BUILDDIR)/* && echo "OK"
 
$(CCOBJSFILE) :
	@echo CCOBJS=`ls $(SRCDIR)/*.c` > $(CCOBJSFILE)

$(STATIC_TARGET) : $(LDOBJS)
	@echo -n "Linking $^ to $@ ... " && $(AR) cqs $@ $^ && echo "OK"

$(SHARED_TARGET) : $(LDOBJS)
	@echo -n "Linking $^ to $@ ... " && $(CC) -o $@ $^ $(LDFLAGS) && echo "OK"
 
$(BUILDDIR)/%.dep : $(SRCDIR)/%.c
	@$(CPP) $(CCFLAGS) -MM -MT $(@:.dep=.o) -o $@ $<
 
$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@echo -n "Building $< ... " && $(CC) $(CCFLAGS) -c -o $@ $< && echo "OK"
 
-include $(DEPEND)

