#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define NIT_SHORT_NAMES
#include <nit/socket.h>

int
main(int argc, char *argv[])
{
	Nit_joint *jnt = joint_connect("kwt");
	int32_t msg_size;
	char buf[256];

	(void) argc;
	(void) argv;

	fgets(buf, 255, stdin);
	msg_size = strlen(buf);
	printf("msg_size: %" PRIi32 "\n",
	       msg_size);
	joint_send(jnt, buf, msg_size);
	sleep(10);
	joint_free(jnt);
}
