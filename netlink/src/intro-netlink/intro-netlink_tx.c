/*****************************
 * intro-netlink.c
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

#define NETLINK_TEST 17
#define MAX_PAYLOAD 2048


//Static and Global variables


void netlink_usage (void) {
	//Print the usage of the command line
	//TODO Complete this function
}


//main function
int main (int argc, char* argv[]) {

	int retVal = 0;
    int srcPid = getpid();
    int dstPid = 55456;//getpid();   //Source and destination point to the same app, just a test

    //First Create the Netlink socket
    int fd = socket(AF_NETLINK, SOCK_RAW, 0/*NETLINK_TEST*/);
    if (fd == -1) {
    	printf("Failed to Create an NL socket \n");
    	return -1;
    }

    //Fill the Source Address structure
    struct sockaddr_nl nlSrcAddr;
    memset(&nlSrcAddr, 0, sizeof (struct sockaddr_nl));
    nlSrcAddr.nl_family = AF_NETLINK;
    nlSrcAddr.nl_groups = 0;
    nlSrcAddr.nl_pid = srcPid;
    nlSrcAddr.nl_pad = 0;

    //Bind to the source Address
    retVal = bind(fd, (struct sockaddr*)&nlSrcAddr, sizeof(struct sockaddr_nl));
    if (retVal == -1){
    	printf("Failed to Bind ... \n");
    	printf("Error# = %d\n", errno);
    	return -1;
    }

    //Fill the Destination Address
    struct sockaddr_nl nlDstAddr;
    memset(&nlDstAddr, 0, sizeof(struct sockaddr_nl));
    nlDstAddr.nl_family = AF_NETLINK;
    nlDstAddr.nl_groups = 0;
    nlDstAddr.nl_pid = dstPid;
    nlDstAddr.nl_pad = 0;

    //Fill the message Header
    struct nlmsghdr *nlMsgHdr;
    nlMsgHdr = (struct nlmsghdr *)malloc (100);
    memset(nlMsgHdr, 0, 100);
    nlMsgHdr->nlmsg_len = 100;
    nlMsgHdr->nlmsg_pid = dstPid;
    nlMsgHdr->nlmsg_flags = 0;
    nlMsgHdr->nlmsg_seq = 1;
    nlMsgHdr->nlmsg_type = 0;  //Application dependent
    strcpy(NLMSG_DATA(nlMsgHdr), "Hello, there....this is my first message\n");


    //Fill the iov structure
    struct iovec iov;
    iov.iov_base = (void *)nlMsgHdr;
    iov.iov_len = nlMsgHdr->nlmsg_len;

    //Fill in the outer MsgHdr
    struct msghdr msgHdr;
    memset(&msgHdr, 0, sizeof(struct msghdr));
    msgHdr.msg_name = (void *) &nlDstAddr;
    msgHdr.msg_namelen = sizeof(nlDstAddr);
    msgHdr.msg_iov = &iov;
    msgHdr.msg_iovlen = 1;

    //Send the message
    retVal = sendmsg(fd, &msgHdr, 0);
    if (retVal == -1) {
    	printf("Failed to send the message....\n");
    	printf("Error No.: %d \n", errno);
    	printf("Error String: %s\n", strerror(errno));
    	return -1;
    }

    printf("The number of sent characters are: %d\n",retVal);

    //Close the socket
    close (fd);

	return 0;
}
