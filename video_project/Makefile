# use pkg-config for getting CFLAGS and LDLIBS
FFMPEG_LIBS=    libavdevice

CFLAGS += -Wall -g
CFLAGS := $(shell pkg-config --cflags $(FFMPEG_LIBS)) $(CFLAGS)
LDLIBS := $(shell pkg-config --libs $(FFMPEG_LIBS)) $(LDLIBS)

EXECUTABLE=       main

OBJS=$(addsuffix .o,$(EXECUTABLE))

.phony: all clean

all: $(OBJS) $(EXECUTABLE)

clean:
	$(RM) $(EXECUTABLE) $(OBJS)
	$(RM) output/*
