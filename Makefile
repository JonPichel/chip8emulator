CC=gcc
CFLAGS=-g -Wall
LDFLAGS=-lncurses -lmenu -lrt

SRCDIR=src
INCDIR=include
OBJDIR=objs
DEPDIR=deps
BINDIR=bin

BIN=$(BINDIR)/main

# Match all .c files in the src directory
SRCS=$(wildcard $(SRCDIR)/*.c)
# Substitute everything in the SRCS variable that matches the first pattern, with the second pattern
OBJS=$(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))
# I think this does the same as the patsubst
DEPS=$(SRCS:$(SRCDIR)/%.c=$(DEPDIR)/%.d)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $(BIN)

# | -> order-only prerequisites, target won't be rebuilt if the deps dir touchstamp changes
$(OBJDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d | $(DEPDIR) $(OBJDIR)
	$(CC) -MMD -MF $(DEPDIR)/$*.d $(CFLAGS) -I $(INCDIR) -c $< -o $@

$(DEPDIR):
	mkdir -p $(DEPDIR)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

$(DEPS):

clean:
	rm -r $(OBJDIR)
	rm -r $(DEPDIR)
	rm -r $(BINDIR)

# Include the generated dependency files
-include $(DEPS)

