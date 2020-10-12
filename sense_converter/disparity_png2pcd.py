import cv2
import os
import glob
import re
import numpy as np
import par_1280x720_1280x720_S0 as hd_para
import par_1280x720_768x480_S0 as vga_para
# need to modify
baseline = 60.4341964

camera_parameter = {
	'hd' : {	'cx' : hd_para.float_cx,
				'cy' : hd_para.float_cy,
				'fx' : hd_para.float_focal_length,
	},
	'vga' : {	'cx' : vga_para.float_cx,
				'cy' : vga_para.float_cy,
				'fx' : vga_para.float_focal_length,
	}
}

public_header = "# .PCD v.7 - Point Cloud Data file format\n\
VERSION .7\n\
FIELDS x y z\n\
SIZE 4 4 4\n\
TYPE F F F\n\
COUNT 1 1 1\n\
WIDTH XXXX\n\
HEIGHT 1\n\
VIEWPOINT 0 0 0 1 0 0 0\n\
POINTS XXXX\n\
DATA ascii\n"

def depthToPCD(file_path, dst_dir):
	try:
		os.mkdir(dst_dir)
	except:
		print('%s existed.' % dst_dir)
	print(file_path, dst_dir)
	dst_file_name = dst_dir + '/' + os.path.splitext(os.path.basename(file_path))[0] + '.pcd'
	print('output ' , dst_file_name)
	with open(file_path, 'rb') as f:
		img = cv2.imread(file_path, cv2.CV_16UC1)
		
		height = img.shape[0]#height(rows) of image
		width = img.shape[1]#width(colums) of image
		camera_para = {}
		if width == 1280:
			camera_para = camera_parameter['hd']
		elif width == 640:
			camera_para = camera_parameter['vga']
		else:
			print('err width :', width)
			exit
		print('width: %d \nheight: %d' % (width,height))
		header = public_header
		header = header.replace('XXXX', str(width*height))
		i = 0
		with open(dst_file_name, 'w') as wf:
			wf.write(header)
			with np.nditer(img, op_flags=['readwrite']) as it:
				for disparity in it:
					color = 0
					depth = 0
					if disparity != 0:
						depth = camera_para['fx']*baseline/(disparity/8)
					x = i % width
					y = (int) (i/width)
					pos_z = depth
					pos_x = ((x - camera_para['cx'])*depth)/camera_para['fx']
					pos_y = ((y - camera_para['cy'])*depth)/camera_para['fx']
					i = i + 1
					wf.write('%f %f %f\n' % (pos_x, pos_y, pos_z))
		# cv2.imwrite(dst_file_name, img)

if __name__ == "__main__":

	src_dir = './snapshot'
	import sys
	if len(sys.argv) <3:
		print('[Err] please exec python like -> disparith_png2pcd.py baseline src_dir')
		exit(0)
	src_dir = sys.argv[2]
	baseline = eval(sys.argv[1])
	print('input & output dir : %s' % src_dir)

	file_list = os.listdir(src_dir)
	depth_file_list = []
	for file_name in file_list:
		result = re.match(r'.+_d.png', file_name)
		if result:
			depth_file_list.append(src_dir + '/' + file_name)
			# print(result)
		
	for file_path in depth_file_list:
		depthToPCD(file_path, src_dir)
		# print(file_path)
		