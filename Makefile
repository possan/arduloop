all: images.h

images.h: convert.py logo.png 32x32gradient.bmp
	python convert.py logo.png 32x32gradient.bmp >images.h
