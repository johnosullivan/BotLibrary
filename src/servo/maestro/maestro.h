#ifndef __MAESTRO_H__
#define __MAESTRO_H__

int maestroGetPosition(int fd, unsigned char channel);
int maestroSetTarget(int fd, unsigned char channel, unsigned short target);

#endif /* __MAESTRO_H__ */
