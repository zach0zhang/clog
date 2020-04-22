AUTOMAKE_OPTIONS=foreign
lib_LTLIBRARIES=liblog.la
include_HEADERS=inc/*
INCLUDES=-I./inc -I./cstl/inc 

CSTL_SRC=cstl/src/cstl_lib.c \
	 cstl/src/cstl_map.c \
	 cstl/src/cstl_rb.c 
 
liblog_la_SOURCES = src/log.c $(CSTL_SRC)
liblog_la_LDFLAGS = -version-info 1:1:1
