RD = rm -rf
ifdef OS
	RD = rd /s /q
	LDFLAGS = -static-libstdc++ -static-libgcc -static
endif

CXX = g++
CXXFLAGS  = -O3
CXXFLAGSD = -g3 -ggdb -Wall
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

build:	$(BINDIR) $(addprefix $(BINDIR)/$(RLSDIR)/, $(OBJDIR)) $(addprefix $(BINDIR)/$(RLSDIR)/, psarc)

debug:	$(BINDIR) $(addprefix $(BINDIR)/$(DBGDIR)/, $(OBJDIR)) $(addprefix $(BINDIR)/$(DBGDIR)/, psarc)

$(BINDIR)/$(RLSDIR)/psarc:	$(addprefix $(BINDIR)/$(RLSDIR)/$(OBJDIR)/, $(OBJS))
		$(CXX) $(LDFLAGS) -o $(BINDIR)/$(RLSDIR)/psarc $^ $(LDLIBS)

$(BINDIR)/$(DBGDIR)/psarc:	$(addprefix $(BINDIR)/$(DBGDIR)/$(OBJDIR)/, $(OBJS))
		$(CXX) $(LDFLAGS) -o $(BINDIR)/$(DBGDIR)/psarc $^ $(LDLIBS)

$(BINDIR):
		-mkdir $(BINDIR)

$(BINDIR)/$(RLSDIR)/$(OBJDIR): 
		-cd $(BINDIR) && mkdir $(RLSDIR) && cd $(RLSDIR) && mkdir $(OBJDIR)

$(BINDIR)/$(DBGDIR)/$(OBJDIR): 
		-cd $(BINDIR) && mkdir $(DBGDIR) && cd $(DBGDIR) && mkdir $(OBJDIR)

$(BINDIR)/$(RLSDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

$(BINDIR)/$(DBGDIR)/$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGSD) -MMD -c $< -o $@

clean:
	$(RD) $(BINDIR)
	
-include $(addprefix $(OBJDIR)/, $(DEPS))
