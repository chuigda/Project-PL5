ifndef CC
	CC = gcc
endif
ifndef CFLAGS
	CFLAGS = -Wall -Wextra \
		-Wno-pedantic \
		-Wno-cast-function-type \
		-Wno-char-subscripts \
		-g \
		$(EXTRA_CFLAGS)
endif

ifndef WIN32
	MSCM_SHARED_LIB_NAME = libmscm.so
	SHARED_LIB_NAME = libjlparse.so
else
	MSCM_SHARED_LIB_NAME = libmscm.dll
	SHARED_LIB_NAME = libjlparse.dll
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

$(SHARED_LIB_NAME): ../../$(MSCM_SHARED_LIB_NAME) jlparse.o
	$(call LOG,LINK,$(SHARED_LIB_NAME))
	@$(CC) $(CFLAGS) jlparse.o \
		-L../.. $(EXTRA_LD_SEARCH_PATH) \
		-lmscm -ljulia \
		-shared -o $(SHARED_LIB_NAME)

jlparse.o: jlparse.c
	$(call COMPILE,$<,$@)
