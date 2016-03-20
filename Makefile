CC = gcc
LEX = flex
YACC = yacc

CFLAGS ?= -Wall -Wextra -Wno-unused-function -D_GNU_SOURCE -O2
CPPFLAGS += -Iincludes
LDFLAGS += -lfl
YFLAGS += -d
EXEC = lpisc

LEX_FILES = $(shell ls lex/*.l)
YACC_FILES = $(shell ls yacc/*.y)
C_FILES = $(shell ls src/*.c) $(YACC_FILES:%.y=%.tab.c) $(LEX_FILES:%.l=%.yy.c)
O_FILES = $(C_FILES:%.c=%.o)

.PHONY: clean
.SECONDARY:
%.tab.c %.tab.h: %.y
	@printf "YACC\n"
	$(YACC) -o $@ $(YFLAGS) $<

%.yy.c : %.l
	@printf "FLEX\n"
	$(LEX) -o $@ $<

%.tab.o : %.tab.c
	@printf "COMPILING\n"
	$(COMPILE.c) $(OUTPUT_OPTION) $<

%.yy.o : CPPFLAGS += -Iyacc
%.yy.o : %.yy.c
	@printf "COMPILING\n"
	$(COMPILE.c) $(OUTPUT_OPTION) $<

$(EXEC) : $(O_FILES)
	@printf "LINKING\n"
	$(CC) $(CFLAGS) $^ $(CPPFLAGS) $(LDFLAGS) $(OUTPUT_OPTION) $(LOADLIBES) $(LDLIBS)

debug: CFLAGS:=$(filter-out -O2,$(CFLAGS))
debug: CFLAGS += -g -DDEBUG
debug: YFLAGS += -v
debug: $(EXEC)

clean:
	@printf "CLEAN\n"
	@printf "\n"
	$(RM) **/*.o
	$(RM) **/*.yy.c
	$(RM) **/*.h.ghc
	$(RM) **/*.out
	$(RM) **/*.tab.c
	$(RM) **/*.tab.h
	$(RM) **/*.output
	$(RM) $(EXEC)

