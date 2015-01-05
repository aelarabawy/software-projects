/*****************************
 * intro-netlink_rx.c
 * Author: Ahmed I. ElArabawy
 * Created on: 11/26/2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <errno.h>

#define MAX_PAYLOAD_LEN 2048


//Static and Global variables


void netlink_usage (void) {
	//Print the usage of the command line
	//TODO Complete this function
}


//main function
int main (int argc, char* argv[]) {

	int retVal;

    //First Create the Netlink socket
    int fd = socket(AF_NETLINK, SOCK_RAW, 0/*Replace this with the protocol name NETLINK_xxxxx*/);
    if (fd == -1) {
    	printf("Failed to Create an NL socket for the Reciever.... \n");
    	printf("Error: %s (%d)\n",strerror(errno), errno);
    	return -1;
    }

    //Define the Source address to bind to
    struct sockaddr_nl nlSrcAddr;
    int srcPid = 55456; //replace this with a unique number ... normally getpid();

    //Fill the Source Address structure
    memset(&nlSrcAddr, 0, sizeof (struct sockaddr_nl));
    nlSrcAddr.nl_family = AF_NETLINK;
    nlSrcAddr.nl_groups = 0;  //Put here multicast groups subscribed to if any
    nlSrcAddr.nl_pid = srcPid;
    nlSrcAddr.nl_pad = 0;

    //Bind to the source Address
    retVal = bind(fd, (struct sockaddr*)&nlSrcAddr, sizeof(struct sockaddr_nl));
    if (retVal == -1){
    	printf("Failed to Bind ... \n");
    	printf("Error# = %d\n", errno);
    	return -1;
    }

    //Prepare for the reception
	char buf [MAX_PAYLOAD_LEN]; //Buffer carrying the received message (including the NL header)
    struct nlmsghdr *nlMsgHdr = (struct nlmsghdr *)buf;

    struct iovec iov;
    iov.iov_base = (void *)nlMsgHdr;
    iov.iov_len = MAX_PAYLOAD_LEN;

    struct msghdr msg;  //Message header pointing to the buffer
    msg.msg_name = (void *) &nlSrcAddr;
    msg.msg_namelen = sizeof(nlSrcAddr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;

    //Wait for messages to receive
    while (1) {
    	printf("Receiving a Message:\n");

    	ssize_t ret = recvmsg(fd, &msg, 0);
    	if (ret < 0) {
    		perror("recvmsg");
    		printf("Error in recvmsg\n");
    		printf("Error Number: %d : %s", errno, strerror(errno));
    		return -1;
    	}
        printf("Received a message: %s \n", NLMSG_DATA(nlMsgHdr));

    }

	return 0;
}
