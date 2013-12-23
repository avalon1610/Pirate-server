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
    unsigned int digit = 0;  // ��¼ip��ַ4������
    int dotNum = 0;    // num of dot
    int digitNum = 0;  // num of digit
    char input = 0;
	int ipIdx;
	
	if(ip_address==NULL)
		return 0;
	for(ipIdx =0;ipIdx<ip_len;++ipIdx)
	{
		input = ip_address[ipIdx];
		if(input >= '0' && input <= '9') // ����
		{
			++ digitNum;
			digit = 10 * digit + (input - '0');
			if(digit > 255) // ���ַǷ��򳤶ȹ���
			{
				return 0;
			}
		}else if(input == '.')// ���㣬�ϲ����ֽ��
		{
			++ dotNum;
			if(dotNum > digitNum)// ���� ..0.1 or 4..0.1
			{
				return 0;
			}else // �ϲ�
			{
				changed_ip = (changed_ip<<8) | digit;
				digit = 0;
			}
		}else if(input == '\0') // �������������������ؽ��
			{
				if(dotNum != 3)
					{
						return 0;
					}else
					{
						changed_ip = (changed_ip<<8) + digit;
						return changed_ip;
					}
			}else // �Ƿ�����
				{
					return 0;
				}
	}
	




}



