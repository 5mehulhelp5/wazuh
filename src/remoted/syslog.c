/*   $OSSEC, syslog.c, v0.4, 2005/11/10, Daniel B. Cid$   */

/* Copyright (C) 2003,2004,2005 Daniel B. Cid <dcid@ossec.net>
 * All right reserved.
 *
 * This program is a free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation
 */



#include "shared.h"
#include "os_net/os_net.h"

#include "remoted.h"





/* OS_IPNotAllowed, v0.1, 2005/02/11 
 * Checks if an IP is not allowed.
 */
int OS_IPNotAllowed(char *srcip)
{
    if(logr.denyips != NULL)
    {
        if(OS_IPFoundList(srcip, logr.denyips))
        {
            return(1);
        }
    }
    if(logr.allowips != NULL)
    {
        if(OS_IPFoundList(srcip, logr.allowips))
        {
            return(0);
        }
    }

    /* If the ip is not allowed, it will be denied */
    return(1);
}


/** void HandleSyslog(int position) v0.2
 * Handle syslog connections
 */
void HandleSyslog(int position)
{
    char buffer[OS_MAXSTR +2];
    char srcip[IPSIZE +1];

    int recv_b;

    struct sockaddr_in peer_info;
    socklen_t peer_size;


    /* setting peer size */
    peer_size = sizeof(peer_info);


    /* Initializing some variables */
    memset(buffer, '\0', OS_MAXSTR +2);


    /* Infinit loop in here */
    while(1)
    {
        /* Receiving message  */
        recv_b = recvfrom(logr.sock, buffer, OS_MAXSTR, 0, 
                (struct sockaddr *)&peer_info, &peer_size);

        /* Nothing received */
        if(recv_b <= 0)
            continue;


        /* Setting the source ip */
        strncpy(srcip, inet_ntoa(peer_info.sin_addr), IPSIZE);
        srcip[IPSIZE] = '\0';


        /* Checking if IP is allowed here */
        if(OS_IPNotAllowed(srcip))
        {
            merror(DENYIP_ERROR,ARGV0,srcip);
        }

        else if(SendMSG(logr.m_queue, buffer,srcip,
                        "syslog",
                        SYSLOG_MQ) < 0)
        {
            merror(QUEUE_ERROR,ARGV0,DEFAULTQUEUE);
            if((logr.m_queue = StartMQ(DEFAULTQUEUE,READ)) < 0)
            {
                ErrorExit(QUEUE_FATAL,ARGV0,DEFAULTQUEUE);
            }
        }
    }
}



/* EOF */
