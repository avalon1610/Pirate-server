#include  "./include/network.h"


int send_storm(libnet_t *lib_net,int size,int pcap_size)
{	int c;
	int send=0;
	while(send<size)
		{
			c =	libnet_write(lib_net);
			send=send+pcap_size;
			  if (c == -1)
    		{
       			 fprintf(stderr, "Write error: %s\n", libnet_geterror(lib_net));
        			return -1;
    		}
		}
	return 1;

}


u_int32_t Get_long_address(char *ip_address)
{

	int ip_len=16;
	u_int32_t changed_ip;
    unsigned int digit = 0;  // 记录ip地址4个整数
    int dotNum = 0;    // num of dot
    int digitNum = 0;  // num of digit
    char input = 0;
	int ipIdx;
	
	if(ip_address==NULL)
		return 0;
	for(ipIdx =0;ipIdx<ip_len;++ipIdx)
	{
		input = ip_address[ipIdx];
		if(input >= '0' && input <= '9') // 数字
		{
			++ digitNum;
			digit = 10 * digit + (input - '0');
			if(digit > 255) // 数字非法或长度过长
			{
				return 0;
			}
		}else if(input == '.')// 遇点，合并部分结果
		{
			++ dotNum;
			if(dotNum > digitNum)// 诸如 ..0.1 or 4..0.1
			{
				return 0;
			}else // 合并
			{
				changed_ip = (changed_ip<<8) | digit;
				digit = 0;
			}
		}else if(input == '\0') // 结束符，检查点数，返回结果
			{
				if(dotNum != 3)
					{
						return 0;
					}else
					{
						changed_ip = (changed_ip<<8) + digit;
						return changed_ip;
					}
			}else // 非法输入
				{
					return 0;
				}
	}
	




}



