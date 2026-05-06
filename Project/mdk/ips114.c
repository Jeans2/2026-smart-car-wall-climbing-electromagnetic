#include "ips114.h"

void ips114_show(void)
{
	ips114_show_string(0, 16 * 0, "p:");
	ips114_show_float(30, 16 * 0, pitch, 3, 1);
}
