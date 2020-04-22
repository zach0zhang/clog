#include <stdlib.h>
#include <stdint.h>

#include "log.h"



int main(int argc, char *argv[])
{
	char *file1 = "./tmp/log1.txt";
	char *file2 = "./tmp/log2.txt";
	log(LOG_NOTICE, "FILE:%s FUNC:%s LINE %d, test log starting\n", __FILE__, __func__, __LINE__);
	for (int i = 0; i < 100; i++) {
		log(LOG_ERR, "loop%d:test log to ./tmp/log1.txt\n", i);
		log_to_file(LOG_ERR, file1, "loop%d, test log to file %s\n", i, __func__);
	
	//_log(LOG_ERR, file, "test1\n");
	//_log(LOG_ERR, file, "test2\n");
		log(LOG_ERR, "loop%d test log to ./tmp/log2.txt\n", i);
		log_to_file(LOG_ERR, file2, "loop%d, test log to file%s\n", i, __func__);
	}
	//printf("2 ok\n");
	log_uninit();	
	return 0;
}
