TARGET=andy
CODE=./src

CC=gcc
DEPFLAGS=-MP -MD
CFLAGS=$(foreach D,$(CODE),-I $(D)) $(DEPFLAGS)

CFILES=$(foreach D,$(CODE),$(wildcard $(D)/*.c))
OBJECTS=$(patsubst %.c,%.o,$(CFILES))
DEPENDENCIES=$(patsubst %.c,%.d,$(CFILES))

all:${TARGET}

${TARGET}:${OBJECTS}
	${CC} -o $@ $^

%.o:%.c
	${CC} ${CFLAGS} -c -o $@ $< 

clean:
	rm -rf $(TARGET) $(OBJECTS) $(DEPENDENCIES)

-include $(DEPFILES)

.PHONY: all clean 