CC      := clang
CCFLAGS := -O3 -Wall -Wextra -pedantic -Wno-sign-compare
FPS     ?= 24

all:
	mkdir -p tmp
	$(CC) $(CCFLAGS) -o conway conway.c 
	./conway
	ffmpeg -framerate $(FPS) -i './tmp/output_%04d.ppm' -c:v libx264 -pix_fmt yuv420p -vf "crop=trunc(iw/2)*2:trunc(ih/2)*2:0:0, scale=1280:720:sws_flags=neighbor" conway.mp4
	# ffmpeg -framerate $(FPS) -i './tmp/output_%04d.ppm' -c:v libx264 -pix_fmt yuv420p -vf "crop=trunc(iw/2)*2:trunc(ih/2)*2:0:0, scale=1280:720" conway.mp4

tidy:
	clang-format -i conway.c

clean:
	rm -rf tmp conway /tmp/*.ppm conway.mp4
