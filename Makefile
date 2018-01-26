SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
DEPDIR := .d
$(shell mkdir -p $(DEPDIR) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@
CFLAGS=-g -O0 -DDEBUG -Wall -Werror -Wextra
CFLAGS+=-Wno-unused-parameter -Wno-unused-variable
CFLAGS+=$(shell pkg-config --cflags gtk+-2.0)
LDFLAGS=$(shell pkg-config --libs gtk+-2.0)
all: rawview

rawview: $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@

%.o:%.c 
%.o:%.c $(DEPDIR)/%.d
	$(CC) -c $(CFLAGS) $(DEPFLAGS) $< -o $@
	$(POSTCOMPILE)

clean:
	@rm -f $(OBJS)
	@rm -f rawview
$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

include $(wildcard $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS))))
