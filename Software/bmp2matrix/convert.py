import os
import Image
import numpy
import PIL.ImageOps as ImageOps
counter = 2;

bitmap_code = open('icon.c','w')
bitmap_code.write('const unsigned char Icon%d []  = {'%counter)
for filename in os.listdir("."):
	if "select" in filename:
		print filename
		im = Image.open(filename)
		im = im.convert('L')
		#im = ImageOps.invert(im)
		pix = numpy.array(im)
		for x in pix:
			for y in x:
				bitmap_code.write(str(y)+',')
				pass
			pass
			bitmap_code.write("\n")
		counter +=1
bitmap_code.seek(bitmap_code.tell()-1)
bitmap_code.write("};\n")

