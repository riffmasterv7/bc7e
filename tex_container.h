#pragma once

#include <vector>
#include <cstdint>

const char* get_file_extension(const char* filename);

struct bc7_block
{
	uint64_t m_vals[2];
};

typedef std::vector<bc7_block> bc7_block_vec;

bool save_bc7(const char* pFilename, uint32_t width, uint32_t height, const bc7_block* pBlocks, bool srgb);