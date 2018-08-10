CC := gcc
LD := $(CC)
CFLAGS += -D_DEFAULT_SOURCE # for daemon(3)
CFLAGS += -D_POSIX_C_SOURCE=200809L # for getopt(3p)
CFLAGS += -std=c11 -Wall -Werror
CFLAGS += -MT $@ -MMD -MP -MF tmp/$*.d

bins := coma nameable

all: coma nameable
clean:
	@test ! -d tmp || rm -r tmp
	@$(foreach bin,$(bins),test ! -e $(bin) || rm $(bin);)
.PHONY: all clean

%: tmp/%.o
	$(LD) -o $(@) $(LDFLAGS) $(^)
tmp/%.o: %.c
	@mkdir -p tmp
	$(CC) -c -o $(@) $(CFLAGS) $(^)
tmp/%.d: ;
	@mkdir -p tmp

.PRECIOUS: tmp/%.d
include $(foreach bin,$(bins),tmp/$(bin).d)
