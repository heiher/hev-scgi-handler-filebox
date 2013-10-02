# Makefile for hev-scgi-server
 
CPP=cpp
CC=gcc
PKG_DEPS=glib-2.0 gio-2.0
CCFLAGS=-O3 -fPIC `pkg-config --cflags $(PKG_DEPS)` -I../hev-scgi-server-library/include
LDFLAGS=-shared `pkg-config --libs $(PKG_DEPS)` -L../hev-scgi-server-library/bin -lhev-scgi-server
 
SRCDIR=src
BINDIR=bin
BUILDDIR=build

TARGET=$(BINDIR)/libhev-scgi-server-filebox.so

LDOBJS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o,$(wildcard src/*.c))
DEPEND=$(LDOBJS:.o=.dep)
 
all : $(TARGET)
 
clean : 
	@echo -n "Clean ... " && $(RM) $(BINDIR)/* $(BUILDDIR)/* && echo "OK"
 
$(TARGET) : $(LDOBJS)
	@echo -n "Linking $^ to $@ ... " && $(CC) -o $@ $^ $(LDFLAGS) && echo "OK"
 
$(BUILDDIR)/%.dep : $(SRCDIR)/%.c
	@$(CPP) $(CCFLAGS) -MM -MT $(@:.dep=.o) -o $@ $<
 
$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@echo -n "Building $< ... " && $(CC) $(CCFLAGS) -c -o $@ $< && echo "OK"
 
-include $(DEPEND)

