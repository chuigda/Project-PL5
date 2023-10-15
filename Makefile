ifndef CC
	CC = gcc
endif
ifndef AR
	AR = ar
endif
ifndef RANLIB
	RANLIB = ranlib
endif
ifndef CFLAGS
	CFLAGS = -Wall -Wextra -Wno-pedantic -g $(EXTRA_CFLAGS)
endif

ifndef WIN32
	SHARED_LIB_NAME = libmscm.so
	EXECUTABLE_NAME = mscm
else
	SHARED_LIB_NAME = libmscm.dll
	EXECUTABLE_NAME = mscm.exe
endif

define LOG
	@printf '\t%s\t%s\n' $1 $2
endef

define COMPILE
	$(call LOG,CC,$1)
	@$(CC) $(CFLAGS) $1 \
		-Iconfig -Iinclude -Isrc/include \
		-fPIC -c -o $2
endef

HEADER_FILES = $(wildcard include/*.h) $(wildcard src/include/*.h)
SOURCE_FILES = $(wildcard src/*.c)
OBJECT_FILES := $(patsubst src/%.c,%.o,$(SOURCE_FILES))

.PHONY: mscm-phony mscm-log
mscm-phony: libmscm-phony mscm-log $(EXECUTABLE_NAME)

mscm-log:
	@echo Building executable $(EXECUTABLE_NAME)

$(EXECUTABLE_NAME): $(HEADER_FILES) main.o
	$(call LOG,LINK,$(EXECUTABLE_NAME))
	@$(CC) $(CFLAGS) main.o -L. -lmscm -o $(EXECUTABLE_NAME)

main.o: main.c
	$(call COMPILE,$<,$@)

.PHONY: libmscm-phony libmscm-log
libmscm-phony: libmscm-log $(SHARED_LIB_NAME)

libmscm-log:
	@echo Building shared library $(SHARED_LIB_NAME)

$(SHARED_LIB_NAME): $(OBJECT_FILES)
	$(call LOG,LINK,$(SHARED_LIB_NAME))
	@$(CC) $(CFLAGS) $(OBJECT_FILES) -fPIC -shared -o $(SHARED_LIB_NAME)

%.o: src/%.c $(HEADER_FILES)
	$(call COMPILE,$<,$@)

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.a
	rm -f *.so
	rm -f *.dll
	rm -f *.exe
	rm -f *.core
	rm -f mscm
	rm -f vgcore*
	rm -f core*
