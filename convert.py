#
# Convert images to Arduboy format, scan 8 rows to one byte
#

import os, sys
from PIL import Image

def convert_file(filename):
	filebase = os.path.splitext(filename)[0]

	im = Image.open(filename)
	if not im:
		print "// Failed to load %s" % (filename)
		return

	w = im.size[0]
	h = im.size[1]

	print "//"
	print "// %s (%d x %d pixels)" % (filename, w, h)
	print "//"

	pix = im.load()
	# print pix

	out = []

	def bit(x,y):
		if y >= h:
			return 0;
		if type(pix[x, y]) is tuple: # RGB(A) input
			if pix[x, y][0] > 0:
				return 1
			return 0
		if pix[x, y] > 0: # Palette input
			return 1
		return 0

	for j in range(0, h, 8):
		for i in range(0, w):
			b = 0
			for r in range(0, 8):
				b |= (bit(i, j+r) << r)
			out.append("0x%02X" % (b))

	commaseparated = ",".join(out)

	print "// Example: arduboy.drawBitmap(x, y, __%s, %d, %d, WHITE)" % (filebase, w, h);
	print "//"
	print ""
	print "PROGMEM const unsigned char __%s[] = { %s };" % (filebase, commaseparated)
	print ""

if __name__ == "__main__":
	if len(sys.argv) < 2:
		print "Syntax: convert.py [file1.png] [file2.jpg] [file3.bmp] ... >images.h"
	else:
		for k in sys.argv[1:]:
			convert_file(k)
