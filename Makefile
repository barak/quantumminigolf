EXE = quantumminigolf
all: $(EXE)

CC = $(CXX)
CPPFLAGS += -DLINUX
CPPFLAGS += -I/usr/include/SDL
LOADLIBES +=  -lSDL -lSDL_ttf -lfftw3f -lm

CXXFLAGS += -Wall -Wno-unused

CXXFLAGS += -O2
# CXXFLAGS += -O3			# unclear benefit over -O2
# CXXFLAGS += -fomit-frame-pointer	# unclear benefit
# CXXFLAGS += -finline-functions	# unclear benefit
# CXXFLAGS += -funroll-all-loops	# unclear benefit
# CXXFLAGS += -malign-double		# causes error for arch amd64 -m64

OBJS += quantumminigolf.o
OBJS += Renderer.o
OBJS += ClassicSimulator.o
OBJS += QuantumSimulator.o
OBJS += SoftwareTracker.o
OBJS += Tracker.o
OBJS += TrackSelector.o

$(EXE): $(OBJS)

.PHONY : all clean install uninstall

clean: ; -rm -f $(OBJS) $(EXE) $(dfiles)

prefix=/usr/local
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
datarootdir=$(prefix)/share
datadir=$(datarootdir)
qdatadir=$(datarootdir)/quantumminigolf

CPPFLAGS += -DQDATADIR='"$(qdatadir)"'

INSTALL = install
INSTALL_PROGRAM = $(INSTALL) --mode a+rx,ug+w
INSTALL_DIR = mkdir --parent
INSTALL_DATA = $(INSTALL) --mode a+r,ug+w

install:: $(EXE)
	$(INSTALL_DIR) $(DESTDIR)$(bindir)
	$(INSTALL_PROGRAM) $(EXE) $(DESTDIR)$(bindir)
uninstall::
	-cd $(DESTDIR)$(bindir) && rm -f $(progs)

install::
	$(INSTALL_DIR) $(DESTDIR)$(qdatadir)/tracks
	$(INSTALL_DATA) tracks/tracks.cfg tracks/*.bmp $(DESTDIR)$(qdatadir)/tracks/

install::
	$(INSTALL_DIR) $(DESTDIR)$(qdatadir)/gfx
	$(INSTALL_DATA) gfx/*.bmp $(DESTDIR)$(qdatadir)/gfx/

install::
	$(INSTALL_DIR) $(DESTDIR)$(qdatadir)/fonts
	$(INSTALL_DATA) fonts/*.ttf $(DESTDIR)$(qdatadir)/fonts/

uninstall::
	-rm -r $(DESTDIR)$(qdatadir)/tracks $(DESTDIR)$(qdatadir)/gfx $(DESTDIR)$(qdatadir)/fonts
	-rmdir $(DESTDIR)$(qdatadir)

install-strip:
	$(MAKE) INSTALL_PROGRAM='$(INSTALL_PROGRAM) --strip' install

# unobtrusively account for #include dependencies
override CPPFLAGS += -MMD
dfiles = $(addsuffix .d, $(basename $(OBJS)))
-include $(dfiles)
