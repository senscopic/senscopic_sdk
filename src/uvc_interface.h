#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
	unsigned int writeMMIO(int fd, unsigned int addr,unsigned int value);
	unsigned int readMMIO(int fd, unsigned int addr);
	unsigned int writeI2C(int fd, unsigned int id, unsigned int addr,unsigned int value);
	unsigned int readI2C(int fd, unsigned int id, unsigned int addr);
	unsigned int uvc_exec_python(int fd,const char* file_path );
	unsigned int set_uvc_xu(int fd, uint8_t* in_data);
	unsigned int get_uvc_xu(int fd, uint8_t* out_data);
	unsigned int uvc_read_file(int fd, char* src_file, char* dst_dir );
	unsigned int uvc_write_file(int fd, char* src_file, char* dst_dir );
	int readImu(int fd, uint8_t** ptr);
#ifdef __cplusplus
}
#endif

