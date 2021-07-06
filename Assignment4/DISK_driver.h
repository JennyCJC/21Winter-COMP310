#ifndef DISK_driver_H
#define DISK_driver_H

void initIO();

int getBlockSize();

int partition(const char *name,int totalblocks, int blocksize);

int mountFS(char *name);

int openfile(char *filename);

char* loadBlock();

int readBlock(int file);

int writeBlock(int file, char* data);

int closeFile(int file);



#endif
