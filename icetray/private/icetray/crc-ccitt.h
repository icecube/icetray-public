#ifndef _CRC_CCITT_H
#define _CRC_CCITT_H

extern uint16_t const crc_ccitt_table[256];

static inline uint16_t crc_ccitt_byte(uint16_t crc, const uint8_t c)
{
	return crc_ccitt_table[c ^ (uint8_t)(crc >> 8)] ^ (crc << 8);
}

static inline uint16_t crc_ccitt(uint16_t crc, uint8_t const *buffer, size_t len)
{
	while (len--)
		crc = crc_ccitt_byte(crc, *buffer++);
	return crc;
}

#endif
