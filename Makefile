RD = rm -rf
ifdef OS
	RD = rd /s /q
	LDFLAGS = -static-libstdc++ -static-libgcc -static
endif

CXX = g++
CXXFLAGS  = -O3
LDLIBS = -lz

SRCDIR = src
BINDIR = bin
RLSDIR = rls
DBGDIR = dbg
OBJDIR = obj

SRCS = file.cpp psarc.cpp main.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

all:	build

build:	$(BINDIR) $(RLSDIR) psarc

debug:	RLSDIR := $(DBGDIR)
debug:	CXXFLAGS := -g3 -ggdb -Wall
debug:	$(BINDIR) $(DBGDIR) psarc

psarc:	$(addprefix $(OBJDIR)/, $(OBJS))
		cd $(BINDIR) && cd $(RLSDIR) && $(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BINDIR):
		-mkdir $(BINDIR)

$(RLSDIR): 
		-cd $(BINDIR) && mkdir $(RLSDIR) && cd $(RLSDIR) && mkdir $(OBJDIR)

$(DBGDIR): 
		-cd $(BINDIR) && mkdir $(DBGDIR) && cd $(DBGDIR) && mkdir $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $(BINDIR)/$(RLSDIR)/$@

clean:
	$(RD) $(BINDIR)

-include $(addprefix $(OBJDIR)/, $(DEPS))
