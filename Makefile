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

.PHONY : all clean
clean: ; -rm -f $(OBJS) $(EXE) $(dfiles)

# unobtrusively account for #include dependencies
override CPPFLAGS += -MMD
dfiles = $(addsuffix .d, $(basename $(OBJS)))
-include $(dfiles)
