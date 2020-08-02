#include "gForceAdapter.h"
#include <stdio.h>

enum GF_Gesture g_enumGesture;

void ForceAdapter(unsigned char * buff)
{
	if( *(buff+2) == 0x8f )	 //带Package ID
	{
		g_enumGesture = (*(buff+5));	
	}
	else if( *(buff+2) == 0x0f ) // 不带
	{
		g_enumGesture = (*(buff+4));
	}
	printf("buff %#x\n",buff);
}
