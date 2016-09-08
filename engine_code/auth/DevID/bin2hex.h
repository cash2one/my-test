#ifdef BIN2HEX_HEADER

// 1: success
// 0: failure
int bin2hex(char * dest, const int dest_len, const unsigned char* src, const int src_len);
int hex2bin(unsigned char * dest, const int dest_len, const char* src, const int src_len);

#endif //BIN2HEX_HEADER
