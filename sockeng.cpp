#include "StdAfx.h"
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <winsock.h>
#include <stdlib.h>
#include "Pinger.h"
#include "PingerDlg.h"
#include "sockeng.h"

#define ICMP_ECHO 8
#define ICMP_ECHOREPLY 0
#define ICMP_MIN 8 

typedef struct iphdr 
{
	unsigned int h_len:4;          // length of the header
	unsigned int version:4;        // Version of IP
	unsigned char tos;             // Type of service
	unsigned short total_len;      // total length of the packet
	unsigned short ident;          // unique identifier
	unsigned short frag_and_flags; // flags
	unsigned char  ttl; 
	unsigned char proto;           // protocol (TCP, UDP etc)
	unsigned short checksum;       // IP checksum

	unsigned int sourceIP;
	unsigned int destIP;

} IpHeader;

typedef struct _ihdr 
{
  BYTE i_type;
  BYTE i_code;
  USHORT i_cksum;
  USHORT i_id;
  USHORT i_seq;
  ULONG timestamp;
} IcmpHeader;

#define STATUS_FAILED 0xFFFF
#define DEF_PACKET_SIZE 32
#define MAX_PACKET 1024

#define xmalloc(s) HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,(s))
#define xfree(p)   HeapFree (GetProcessHeap(),0,(p))

extern int gRunning;
extern int gResolving;

sockeng::sockeng(unsigned long to, unsigned long s, unsigned long e,
				 int np,int dl)
{
	num=ntohl(e)-ntohl(s)+1;

	hosts=new long[num];

	timeout=to;
	start=s;
	left=num;
	numpasses=np;
	delay=dl;
}

sockeng::~sockeng(void)
{
	delete hosts;
}


int sockeng::ping(HWND pParam)
{
	WSADATA wsaData;
	fd_set fdset;
	struct timeval tv= {0,delay};

	if (WSAStartup(MAKEWORD(2,2),&wsaData) != 0)
	{
		::PostMessage((HWND)pParam,WM_THREADERROR,0,
			(long)"This program will only work under Windows NT 4.0, or Windows 95/98 with Winsock 2 installed.");
 		return -1;
	}

	if ( LOBYTE( wsaData.wVersion ) != 2 )
	{	
		::PostMessage((HWND)pParam,WM_THREADERROR,0,
			(long)"This program will only work under Windows NT 4.0, or Windows 95/98 with Winsock 2 installed.");
		WSACleanup();
		return -1; 
	}

	SOCKET sockRaw = socket (AF_INET,
					   SOCK_RAW,
					   IPPROTO_ICMP);  

	if (sockRaw == INVALID_SOCKET) 
	{
		::PostMessage((HWND)pParam,WM_THREADERROR,0,
			(long)"Error attempting to open a RAW socket.");
		WSACleanup();
		return -1;
	}

	// Non-blocking
	unsigned long on=1;
    ioctlsocket(sockRaw,FIONBIO,&on);

	int datasize=32+sizeof(IcmpHeader);

	memset(hosts,0,num*sizeof(long));

	struct sockaddr_in dest,from;
	memset(&dest,0,sizeof(dest));
	dest.sin_addr.s_addr=start;
	dest.sin_family = AF_INET;

	char *icmp_data =(char *) xmalloc(1024);
	if (!icmp_data) 
	{
		::PostMessage((HWND)pParam,WM_THREADERROR,0,
			(long)"Can't malloc necessary memory.");
		closesocket(sockRaw);
		WSACleanup( );
		return -1;
	}

	char *recvbuf =(char *) xmalloc(1024);
	if (!recvbuf)
	{
		::PostMessage((HWND)pParam,WM_THREADERROR,0,
			(long)"Can't malloc necessary memory.");
		closesocket(sockRaw);
		xfree(icmp_data);
		WSACleanup( );		
		return -1;
	}

	memset(icmp_data,0,MAX_PACKET);

	IcmpHeader *icmphdr = (IcmpHeader*)icmp_data;

	icmphdr->i_type = ICMP_ECHO;
	icmphdr->i_code = 0;
	icmphdr->i_cksum = 0;
	icmphdr->i_seq = 0;
  
	char *datapart = icmp_data + sizeof(IcmpHeader);
	memset(datapart,'E', datasize - sizeof(IcmpHeader));
	
	struct sockaddr_in cur=dest;

	DWORD endtime=0;
	int j=-1;
	int seq_no=0;
	int s=0;

	int bwrote,bread;
	IpHeader *iphdr;
	int iphdrlen;
	int fromlen=sizeof(from);

	unsigned short int randid=(rand()*GetTickCount())%
					sizeof(unsigned short int);
	
	while((!endtime) || (GetTickCount()<endtime))
	{
		if (gRunning==0)
			break;

		if (!endtime)
		{
			FD_ZERO(&fdset);
			FD_SET(sockRaw,&fdset);
			select(0,NULL,&fdset,NULL,&tv);
			if (FD_ISSET(sockRaw,&fdset))
			{
				j++;
				if (j==num)
				{
					j=0;
					numpasses--;
					if (numpasses==0)
					{
						endtime=GetTickCount()+timeout;
						::PostMessage((HWND)pParam,WM_THREADLISTEN,0,0);
						continue;
					}
				}

				((IcmpHeader*)icmp_data)->i_cksum = 0;
				((IcmpHeader*)icmp_data)->timestamp = GetTickCount();
				((IcmpHeader*)icmp_data)->i_seq = j;
				((IcmpHeader*)icmp_data)->i_id = randid;
				((IcmpHeader*)icmp_data)->i_cksum = checksum((USHORT*)icmp_data, 
													datasize);

				dest.sin_addr.s_addr=htonl(ntohl(cur.sin_addr.s_addr)+j);
				bwrote = sendto(sockRaw,icmp_data,datasize,0,(struct sockaddr*)&dest,
						sizeof(dest));
				::PostMessage((HWND)pParam,WM_THREADPING,0,dest.sin_addr.s_addr);
				if (bwrote == SOCKET_ERROR)
					continue;
			}
		}
		
		FD_ZERO(&fdset);
		FD_SET(sockRaw,&fdset);
		select(0,&fdset,NULL,NULL,&tv);
		if (FD_ISSET(sockRaw,&fdset))
		{
			bread = recvfrom(sockRaw,recvbuf,MAX_PACKET,0,
							(struct sockaddr*)&from,
							&fromlen);
			if (bread == SOCKET_ERROR)
				continue;

			iphdr = (IpHeader *)recvbuf;
			iphdrlen = iphdr->h_len << 2 ; 
			if (bread  < iphdrlen + ICMP_MIN) 
				continue;

			icmphdr = (IcmpHeader*)(recvbuf + iphdrlen);
			if (icmphdr->i_type != ICMP_ECHOREPLY) 
				continue;

			if ((icmphdr->i_id)!=randid)
				continue;

			if (((icmphdr->i_seq)<num) && ((icmphdr->i_seq)>=0))
			{
				if (hosts[icmphdr->i_seq]==0)
					::PostMessage((HWND)pParam,WM_THREADHOSTALIVE,
					(GetTickCount()-(icmphdr->timestamp)),
					from.sin_addr.s_addr);
				hosts[icmphdr->i_seq]=(GetTickCount()-(icmphdr->timestamp))+1;
			}
		}
	}

	::PostMessage((HWND)pParam,WM_THREADDONE,0,0);

	xfree(icmp_data);
	xfree(recvbuf);
	closesocket(sockRaw);

	WSACleanup();
	return 0;
}

long sockeng::getaddr(char *string)
{
	long addr;
	
	addr = inet_addr(string); 
	if (addr == INADDR_NONE)
		return -1;
	return addr;
}

USHORT sockeng::checksum(USHORT *buffer, int size) 
{
  unsigned long cksum=0;

  while(size >1) {
	cksum+=*buffer++;
	size -=sizeof(USHORT);
  }
  
  if(size ) {
	cksum += *(UCHAR*)buffer;
  }

  cksum = (cksum >> 16) + (cksum & 0xffff);
  cksum += (cksum >>16);
  return (USHORT)(~cksum);
}
