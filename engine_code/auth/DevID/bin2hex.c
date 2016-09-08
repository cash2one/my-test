/*
 * bin2hex
 *
 * input:
 *   dest_len: length of dest buffer, 
 *   src     : binary source
 *   src_len : length to encode
 *
 * output:
 *   dest    : hex format content, filled from the first byte
 *
 * return value:
 *   1       : success
 *   0       : failure, usually not enough space to store value
 */
static inline void char2hex(unsigned char* p, const unsigned char c)
{
	unsigned char cc;

	cc=(c&0xf0)>>4;
	if(cc<10)
		*p='0'+cc;
	else
		*p='A'+cc-10;
	p++;

	cc=c&0x0f;
	if(cc<10)
		*p='0'+cc;
	else
		*p='A'+cc-10;
}

int bin2hex(char * dest, const int dest_len, const unsigned char* src, const int src_len)
{
	int i;
	unsigned char * p;

	if(dest_len<1) return 0;
	if(src_len<1) return 0;
	if(dest_len < 2*src_len) return 0;

	i=0; p=(unsigned char*)dest;

	while(i<src_len){
		char2hex(p,src[i]);
		p+=2; i++;
	}

	return 1;
}


/*
 * hex2bin
 *
 * input:
 *   dest_len: length of dest buffer, 
 *   src     : content in hex format
 *   src_len : length to decode
 *
 * output:
 *   dest    : binary content, filled from the first byte
 *
 * return value:
 *   1       : success
 *   0       : failure, usually not enough space to store value
 */
static inline char hex2char(char c)
{
	if( (c>='0')&&(c<='9'))return c-'0';
	if( (c>='A')&&(c<='F'))return c-'A'+10;
	if( (c>='a')&&(c<='f'))return c-'a'+10;
	return -1;
}

int hex2bin(unsigned char * dest, const int dest_len, const char* src, const int src_len)
{
	int i,shift;
	char c;
	unsigned char* p;

	if(dest_len<1)return 0;
	if(src_len<1)return 0;
	if(src_len > 2*dest_len)return 0;
	
	memset(dest,0,dest_len);
	
	i=0; shift=4;
	p=dest;

	while(i<src_len){
		c=hex2char(src[i]);
		if(c<0)return 0;
		*p|=c<<shift;
		shift=4-shift;
		if(shift)p++;
		i++;
	}
	return 1;
}

