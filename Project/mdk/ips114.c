#include "ips114.h"

void ips114_show()
{
	ips114_show_string(0,16*0,"L:");
	ips114_show_string(0,16*1,"LM:");
	ips114_show_string(0,16*2,"RM");
	ips114_show_string(0,16*3,"R:");
	ips114_show_string(0,16*3,"gyro_z:");
	
	ips114_show_float(22,16*0,L,3,1);
	ips114_show_float(22,16*1,LM,3,1);
	ips114_show_float(22,16*2,RM,3,1);
	ips114_show_float(22,16*3,R,3,1);
	

}