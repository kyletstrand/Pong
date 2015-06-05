CXX=g++
CFLAGS=-Wall
LDFLAGS=`pkg-config --libs allegro-5.0 allegro_audio-5.0 allegro_font-5.0 allegro_ttf-5.0 allegro_image-5.0 allegro_acodec-5.0 allegro_primitives-5.0 allegro_dialog-5.0`
INCLUDE=#allegro5/allegro.h allegro5/allegro_image.h allegro5_native_dialog.h
DEBUGMODE=1
OBJS=pong.cxx
OUT=pong

all:pong

clean:
	rm -rf *.o pong

pong: $(OBJS)
	$(CXX) $(OBJS) -o $(OUT) $(INCLUDE) $(CFLAGS) $(LDFLAGS)
