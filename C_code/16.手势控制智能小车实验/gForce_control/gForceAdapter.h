#ifndef __GFORCEADAPTER_H__
#define __GFORCEADAPTER_H__

//数据包格式
//Byte[0]  Byte[1] Byte[2]       Byte[3]    Byte[4]
//0xFF      0XAA  0X0F(bit7=0)	  0x01		 手势指令
//FF AA 8F 02 73 05  代表手势为05：空捏
// Magic number
#define MAGNUM_LOW            ((unsigned char)0xFF)
#define MAGNUM_HIGH           ((unsigned char)0xAA)

/*手势枚举*/ 
// Type and constants for gestures
enum GF_Gesture {
	 GF_RELEASE,
	 GF_FIST,
	 GF_SPREAD,
	 GF_WAVEIN,
	 GF_WAVEOUT,
	 GF_PINCH,
	 GF_SHOOT,
	 GF_UNKNOWN = 0xff
};

extern void ForceAdapter(unsigned char * buff);
extern enum GF_Gesture g_enumGesture;


#endif
