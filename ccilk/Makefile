BUILDDIR := build
PRODUCT := deqn_cilk

SRCDIR := src

HDRS := $(wildcard $(SRCDIR)/*.h)

CSRCS := $(wildcard $(SRCDIR)/*.c)

OBJS := $(CSRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

BINARY := $(BUILDDIR)/$(PRODUCT)

# gc flags:
CC := icc
CCFLAGS_DEBUG := -g -DDEBUG -Wall
CCFLAGS_OPT := -O3
LD := $(CC)

# select optimized or debug
#CCFLAGS := $(CCFLAGS_OPT) $(CPPFLAGS)
CCFLAGS :=  $(CCFLAGS_DEBUG) $(CPPFLAGS)

define maketargetdir
	-@mkdir -p $(dir $@) > /dev/null 2>&1
endef

$(BINARY) : $(OBJS)
	@echo linking $@
	$(maketargetdir)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@echo compiling $<
	$(maketargetdir)
	$(CC) $(CCFLAGS) $(CCINCLUDES) -c -o $@ $<

all : $(BINARY)

clean :
	rm -f $(BINARY) $(OBJS)
	rm -rf $(BUILDDIR)
