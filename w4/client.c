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

	printf("kwt v. a0-w4 d. 11/14/16\n");

	while (1) {
		printf("> ");

		if (!fgets(buf, 255, stdin)) {
			printf("\n");
			break;
		}

		msg_size = strlen(buf);

		if (!joint_send(jnt, buf, msg_size))
			break;
	}

	joint_free(jnt);
}
