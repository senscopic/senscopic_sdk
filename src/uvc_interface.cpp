#if defined(linux) || defined(__linux) || defined(__linux__) 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <libgen.h>
#include <errno.h>

#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/videodev2.h>  
#include <linux/uvcvideo.h>  
#include <linux/usb/video.h>  

#include <linux/ioctl.h>
#include <sys/ioctl.h>

#include "uvc_interface.h"
#include <stdint.h>
#include <unistd.h>

// #define DEBUG

unsigned uvc_xu_ioctrl(int fd, struct uvc_xu_control_query *xu)
{
	int ret;
	// static int count = 0;
	// count = 0;
	do {
		ret = ioctl(fd, UVCIOC_CTRL_QUERY, xu);
		if (ret == -1) {
			// count++;
			// if (count > 100) {
				// fprintf(stderr, "[io err %d] %s, more than %d\n", errno, strerror(errno), count);
				// return ret;
			// }
			switch(errno) {
				case EINTR:
					break;
				case EAGAIN:
					break;
				// default:
					// fprintf(stderr, "[io err %d] %s\n", errno, strerror(errno));
			}
		}
	} while (ret == -1 && ((errno == EINTR) || (errno == EAGAIN)));
}

unsigned int set_uvc_xu(int fd, uint8_t* in_data) {
#ifdef DEBUG
	printf("=================================\n");
	printf("usb package data:\n");
	for(int j=0;j<60;j++)
	{
		printf("%02x ", in_data[j]);
		// printf("%c ",in_data[j]);
		if (j%0x10==0xf)
			printf("\n");
	}
	printf("\n");
	printf("=================================\n");
#endif
	// uint8_t data[60];
	struct uvc_xu_control_query xu;
	// memset(&xu,0x00, sizeof(xu));
	// memset(&data,0x00, sizeof(data));
	// xu.unit = 4;
	// xu.selector = 4;
	// xu.query = UVC_GET_LEN;
	// xu.size = 2;
	// xu.data = data;
	// uvc_xu_ioctrl(fd, &xu);

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_SET_CUR;
	xu.size = 60;
	xu.data = in_data;
	uvc_xu_ioctrl(fd, &xu);
}

unsigned int get_uvc_xu(int fd, uint8_t* out_data) {
#ifdef DEBUG
	printf("=================================\n");
	printf("usb package data:\n");
	for(int j=0;j<60;j++)
	{
		printf("%02x ", out_data[j]);
		// printf("%c ",out_data[j]);
		if (j%0x10==0xf)
			printf("\n");
	}
	printf("\n");
	printf("=================================\n");
#endif
	// uint8_t data[64];
	struct uvc_xu_control_query xu;
	// memset(&xu,0x00, sizeof(xu));
	// memset(&data,0x00, sizeof(data));
	// xu.unit = 4;
	// xu.selector = 4;
	// xu.query = UVC_GET_LEN;
	// xu.size = 2;
	// xu.data = data;
	// uvc_xu_ioctrl(fd, &xu);

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_CUR;
	xu.size = 60;
	xu.data = out_data;
	uvc_xu_ioctrl(fd, &xu);
}
unsigned int writeMMIO(int fd, unsigned int addr,unsigned int value)
{
        struct uvc_xu_control_query xu;
        __u8 data[64];
        int ret;
        memset(&xu,0x00, sizeof(xu));
        memset(&data,0x00, sizeof(data));

        xu.unit = 4;
        xu.selector = 4;
        xu.query = UVC_GET_LEN;
        xu.size = 2;
        xu.data = data;

        uvc_xu_ioctrl(fd, &xu);

        memset(&xu,0x00, sizeof(xu));
        memset(&data,0x00, sizeof(data));
        //write command
        unsigned int cmd = 0x00001400; //write
        data[0] = (cmd>> 0) & 0xff;
        data[1] = (cmd>> 8) & 0xff;
        data[2] = (cmd>>16) & 0xff;
        data[3] = (cmd>>24) & 0xff;

        data[4] = (addr >> 0) & 0xff;
        data[5] = (addr >> 8) & 0xff;
        data[6] = (addr >>16) & 0xff;
        data[7] = (addr >>24) & 0xff;

        data[8] =  (value >> 0) & 0xff;
        data[9] =  (value >> 8) & 0xff;
        data[10] = (value >>16) & 0xff;
        data[11] = (value >>24) & 0xff;

        xu.unit = 4;
        xu.selector = 4;
        xu.query = UVC_SET_CUR;
        xu.size = 60;
        xu.data = data;

        uvc_xu_ioctrl(fd, &xu);

        return 0;

}
unsigned int readMMIO(int fd, unsigned int addr)
{
        struct uvc_xu_control_query xu;
        __u8 data[64];
        int ret;
        memset(&xu,0x00, sizeof(xu));
        memset(&data,0x00, sizeof(data));

        xu.unit = 4;
        xu.selector = 4;
        xu.query = UVC_GET_LEN;
        xu.size = 2;
        xu.data = data;

        uvc_xu_ioctrl(fd, &xu);

        memset(&xu,0x00, sizeof(xu));
        memset(&data,0x00, sizeof(data));

        unsigned int cmd = 0x00001000; //read
        data[0] = (cmd>> 0) & 0xff;
        data[1] = (cmd>> 8) & 0xff;
        data[2] = (cmd>>16) & 0xff;
        data[3] = (cmd>>24) & 0xff;

        data[4] = (addr >> 0) & 0xff;
        data[5] = (addr >> 8) & 0xff;
        data[6] = (addr >>16) & 0xff;
        data[7] = (addr >>24) & 0xff;

        xu.unit = 4;
        xu.selector = 4;
        xu.query = UVC_SET_CUR;
        xu.size = 60;
        xu.data = data;

        uvc_xu_ioctrl(fd, &xu);

        memset(&xu,0x00, sizeof(xu));
        memset(&data,0x00, sizeof(data));

        xu.unit = 4;
        xu.selector = 4;
        xu.query = UVC_GET_LEN;
        xu.size = 2;
        xu.data = data;

        uvc_xu_ioctrl(fd, &xu);

        memset(&xu,0x00, sizeof(xu));
        memset(&data,0x00, sizeof(data));

        xu.unit = 4;
        xu.selector = 4;
        xu.query = UVC_GET_CUR;
        xu.size = 60;
        xu.data = data;

        uvc_xu_ioctrl(fd, &xu);

        return data[11] << 24 |
               data[10] << 16 |
               data[ 9] <<  8 |
               data[ 8] <<  0;

}

unsigned int writeI2C(int fd, unsigned int id, unsigned int addr,unsigned int value)
{
	struct uvc_xu_control_query xu;
	__u8 data[64];
	int ret;
	unsigned int cmd = ('I' << 16) | ('W' << 24); //write
	unsigned int format = 0x21; 
	id   = 0x36; 
	//TODO "I2C id 0x36"
	printf("I2C setting id 0x%02x",id);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_LEN;
	xu.size = 2;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));
	//write command
	unsigned int mask   = 0; 
	data[0] = (cmd>> 0) & 0xff;
	data[1] = (cmd>> 8) & 0xff;
	data[2] = (cmd>>16) & 0xff;
	data[3] = (cmd>>24) & 0xff;


	data[4] = (id     ) & 0xff;
	data[5] = (format ) & 0xff;

	data[6] = (mask >>0) & 0xff;
	data[7] = (mask >>8) & 0xff;

	data[8] = (addr >> 8) & 0xff;
	data[9] = (addr >> 0) & 0xff;

	data[10] = (value >>0) & 0xff;
	data[11] = (value >>8) & 0xff;

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_SET_CUR;
	xu.size = 60;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);
}

unsigned int readI2C(int fd, unsigned int id, unsigned int addr)
{
	struct uvc_xu_control_query xu;
	__u8 data[64];
	int ret;

	id     = 0x36; 
	printf("I2C only setting id 0x%02x",id);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_LEN;
	xu.size = 2;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	unsigned int cmd = ('I' << 16) | ('R' << 24); //read
	unsigned int format = 0x21; 
	unsigned int mask   = 0; 
	data[0] = (cmd>> 0) & 0xff;
	data[1] = (cmd>> 8) & 0xff;
	data[2] = (cmd>>16) & 0xff;
	data[3] = (cmd>>24) & 0xff;


	data[4] = (id     ) & 0xff;
	data[5] = (format ) & 0xff;

	data[6] = (mask >>0) & 0xff;
	data[7] = (mask >>8) & 0xff;

	data[8] = (addr >> 8) & 0xff;
	data[9] = (addr >> 0) & 0xff;

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_SET_CUR;
	xu.size = 60;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_LEN;
	xu.size = 2;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_CUR;
	xu.size = 60;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

#if 0
	printf("\n");
	for(int i=0;i<60;i++)
	{
		printf("%02x ",data[i]);
		if (i%0x10==0xf)
			printf("\n");

	}
#endif

	return data[0];

}
//#define DEBUG
unsigned int uvc_exec_python(int fd, const char* file_path ) {
	struct uvc_xu_control_query xu;
	uint8_t data[64];
	unsigned int cmd = 0;
	char *file_path_p = (char *)file_path;
	// memset(&xu,0x00, sizeof(xu));
	// memset(&data,0x00, sizeof(data));
	// set_uvc_xu(fd, data)
	// xu.unit = 4;
	// xu.selector = 4;
	// xu.query = UVC_GET_LEN;
	// xu.size = 2;
	// xu.data = data;
	// uvc_xu_ioctrl(fd, &xu);
	int size = strlen(file_path) +1;
	while(size > 0) {
		memset(&data,0x00, sizeof(data));
		data[0] = 0;
		data[1] = 0;
		data[2] = 'F';
		data[3] = 'E';
		if (size <= 56) {
			memcpy(&data[4], file_path_p, size);
		}
		else {
			memcpy(&data[4], file_path_p, 56);
		}
		set_uvc_xu(fd, data);
		file_path_p+=56;
		size-=56;
	}
}

unsigned int uvc_read_file(int fd, char* src_file, char* dst_dir )
{
#ifdef DEBUG
	printf("[uvc_read_file] filename %s\n", basename(src_file));
#endif
	struct uvc_xu_control_query xu;
	__u8 data[60];
	int init=0;
	int done=0;
	char fnameBuf[128] = {0};
	sprintf(fnameBuf, "%s", src_file);
	int cmd;
#ifdef DEBUG
	printf("[uvc_read_file] fnameBuf %s, sz: %ld\n", fnameBuf, sizeof(fnameBuf));
#endif
	for(int i=0; i<sizeof(fnameBuf); i+=56) 
	{
		if (init == 0) {
			cmd = 0x0000 | ('F' << 16) | ('R' << 24); //file write
			init = 1;
		} else {
			cmd = 0x0000 | ('F' << 16) | ('C' << 24); //file write
		}
		data[0] = (cmd>> 0) & 0xff;
		data[1] = (cmd>> 8) & 0xff;
		data[2] = (cmd>>16) & 0xff;
		data[3] = (cmd>>24) & 0xff;
		memcpy(&data[4], &fnameBuf[i], 56);
		set_uvc_xu(fd, data);
	}
	get_uvc_xu(fd, data);
	int file_size = *(int*)data;
	memset(fnameBuf, 0, sizeof(fnameBuf));
	sprintf(fnameBuf, "%s/%s", dst_dir, basename(src_file));
#ifdef DEBUG
	printf("[uvc_read_file] read file %s to %s , size %d\n", basename(src_file), fnameBuf, file_size);
#endif
	if (file_size<=0)
		return 1;
	FILE* fp;
	if((fp=fopen(fnameBuf,"wb")) == NULL){
		printf("open file error!!\n");
		system("PAUSE");
		exit(0);
	}    
	while (file_size > 0) {
		memset(data, 0, sizeof(data)*sizeof(data[0]));
		cmd = 0x0000 | ('F' << 16) | ('C' << 24); //file write
		data[0] = (cmd>> 0) & 0xff;
		data[1] = (cmd>> 8) & 0xff;
		data[2] = (cmd>>16) & 0xff;
		data[3] = (cmd>>24) & 0xff;
		set_uvc_xu(fd, data);
		get_uvc_xu(fd, data);
		if (file_size >=60)
			fwrite(data, sizeof(data[0]), sizeof(data), fp);
		else
			fwrite(data, sizeof(data[0]), file_size, fp);
		file_size -= sizeof(data);

	}
	fclose(fp);
}
unsigned int uvc_write_file(int fd, char* src_file, char* dst_dir )
{
	FILE* fp = fopen ( src_file, "rb");
	struct uvc_xu_control_query xu;
	__u8 data[64];
	int init=0;
	int done=0;
	char fnameBuf[128] = {0};
	unsigned char filesize[4];


	unsigned int sz = 0;
	fseek(fp, 0L, SEEK_END);
	sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	filesize[0] = (sz>> 0) & 0xff;
	filesize[1] = (sz>> 8) & 0xff;
	filesize[2] = (sz>>16) & 0xff;
	filesize[3] = (sz>>24) & 0xff;

	unsigned int cnt = 0;

	unsigned int cmd = 0;
	enum fileStatus {name, size, content };
	enum fileStatus status=name;

	//check size > 128
	sprintf(fnameBuf, "%s/%s", dst_dir, basename(src_file));
#ifdef DEBUG
	printf("[uvc_write_file] file_size  %d\n",sz);
	printf("[uvc_write_file] dst_path = %s\n", fnameBuf);
#endif
	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_LEN;
	xu.size = 2;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	while(!done) {
		memset(&xu,0x00, sizeof(xu));
		memset(&data,0x00, sizeof(data));

		if (init == 0) {
			cmd = 0x0000 | ('F' << 16) | ('W' << 24); //file write
			init = 1;
		} else { 
			cmd = 0x0000 | ('F' << 16) | ('C' << 24); //file write
		}
		data[0] = (cmd>> 0) & 0xff;
		data[1] = (cmd>> 8) & 0xff;
		data[2] = (cmd>>16) & 0xff;
		data[3] = (cmd>>24) & 0xff;

		for(int i=4;i<60;i++)
		{
			if(status == name) {
				data[i]=fnameBuf[cnt++];
				if ( cnt == sizeof(fnameBuf)) {
					status = size;
					cnt = 0;
					continue;
				}
			}
			if(status == size) {
				data[i]=filesize[cnt++];
				// printf("0x%02x ", data[i]);
				if ( cnt == sizeof(filesize)) {
					status = content;
					cnt = 0;
					continue;
				}
			}
			if(status == content) {
				int ret;
				ret = fread(&data[i], 1, 1, fp);
				cnt++;
				printf("%3d%%\r",cnt*100/sz);
				if (ret == 0) {
					done = 1;
					break;
				}
			}

		}

		xu.unit = 4;
		xu.selector = 4;
		xu.query = UVC_SET_CUR;
		xu.size = 60;
		xu.data = data;

		uvc_xu_ioctrl(fd, &xu);
	}
}

int readImu(int fd, uint8_t** ptr)
{
	struct uvc_xu_control_query xu;
	static uint8_t data[64];
	int ret;
	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_LEN;
	xu.size = 2;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));
	data[0] = 0;
	data[1] = 0;
	data[2] = 'I';
	data[3] = 'M';

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_SET_CUR;
	xu.size = 60;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);
	///-------------------------------
	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_LEN;
	xu.size = 2;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);

	memset(&xu,0x00, sizeof(xu));
	memset(&data,0x00, sizeof(data));

	xu.unit = 4;
	xu.selector = 4;
	xu.query = UVC_GET_CUR;
	xu.size = 60;
	xu.data = data;

	uvc_xu_ioctrl(fd, &xu);
	*ptr = data;
	return 60;
}

#endif
