#pragma once

#include <stdint.h>

struct PackedData {
	uint32_t	undef;
	char		buffer[0x10];
	uint32_t	undef2;
	uint32_t	data_dst_ptr;
	uint32_t	data_left_len;
	uint32_t	ptr_data_origin;
	uint32_t	size;
	uint32_t	compression_key_value;
	uint32_t	size_of_70002000_segment;
};