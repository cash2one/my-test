#include "tcad.h"
char *chstohex ( char* chs )
{
	char hex[16] = { '0', '1', '2', '3', '4', '5', '6', \
		'7', '8','9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	int len = strlen ( chs );
	char* ascii = NULL ;
	ascii = (char*)calloc ( len * 3 + 1, sizeof(char) );            // calloc ascii

	int i = 0;
	while( i < len )
	{
		ascii[i*2] = hex[(int)( (char)chs[i] / 16 )] ;
		ascii[i*2 + 1] = hex[(int)( (char)chs[i] % 16 )] ;
		++i;
	}

	return ascii;                    // ascii 返回之前未释放
}
// 函数输入16进制字符串，输出对应的字符串
char *hextochs ( char* ascii )
{
	int len = strlen ( ascii ) ;
	if( len%2 != 0 )
		return NULL ;
	char *chs = NULL ;
	chs = (char*)calloc( len / 2 + 1, sizeof(char) );                // calloc chs

	int  i = 0 ;
	char ch[2] = {0};
	while( i < len )
	{
		ch[0] = ( (int)ascii[i] > 64 ) ? ( ascii[i]%16 + 9 ) : ascii[i]%16 ;
		ch[1] = ( (int)ascii[i + 1] > 64 ) ? ( ascii[i + 1]%16 + 9 ) : ascii[i + 1]%16 ;

		chs[i/2] = (char)( ch[0]*16 + ch[1] );
		i += 2;
	}

	return chs ;            // chs 返回前未释放
}
//int main(int argc,char **argv)
//{
//	printf("str=%s\n",hextochs(argv[1]));
//}
