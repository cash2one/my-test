const unsigned long cnCRC_32 = 0x04C10DB7;
unsigned long Table_CRC[256];

static void BuildTable32(unsigned long aPoly)
{
	unsigned long i, j;
	unsigned long nData;
	unsigned long nAccum;

	for (i = 0; i < 256; i++) {
		nData = (unsigned long) (i << 24);
		nAccum = 0;

		for (j = 0; j < 8; j++) {
			if ((nData ^ nAccum) & 0x80000000)
				nAccum = (nAccum << 1) ^ aPoly;
			else
				nAccum <<= 1;
			nData <<= 1;
		}

		Table_CRC[i] = nAccum;
	}
}

int CRC_32(unsigned long crc_offset, const char *aData, unsigned long aSize)
{
	unsigned long i;
	unsigned long nAccum = crc_offset;

	BuildTable32(cnCRC_32);

	for (i = 0; i < aSize; i++) {
		//vsos_debug_out("table:%08x, data:%02x", Table_CRC[( nAccum >> 24 ) ^ *aData], *aData);
		nAccum = (nAccum << 8) ^ Table_CRC[(nAccum >> 24) ^ *aData++];
		//vsos_debug_out("CRC:%08x\r\n", nAccum);
	}
	return (int) (nAccum % 10000);
}
