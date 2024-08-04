CC      := clang
CCFLAGS := -O3 -Wall -Wextra -pedantic -Wno-sign-compare
FPS     ?= 24

all:
	clang-format -i conway.c
	$(CC) $(CCFLAGS) -o conway conway.c 
	./conway

clean:
	rm -rf tmp conway *.ppm

tmp:
	# mkdir tmp
	./conway
	# ffmpeg -framerate $(FPS) -i './tmp/output_%03d.ppm' -c:v libx264 -pix_fmt yuv420p -vf "crop=trunc(iw/2)*2:trunc(ih/2)*2:0:0, scale=1280:720" conway.mp4"""
