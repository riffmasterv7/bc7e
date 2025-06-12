
#include "tex_container.h"

#include "dds_defs.h"
#include "ktx_defs.h"

const char* get_file_extension(const char* filename)
{
	const char* dot = strrchr(filename, '.');
	if (!dot || dot == filename) return "";
	return dot + 1;
}

void save_bc7_ktx(FILE* pFile, uint32_t width, uint32_t height, const bc7_block* pBlocks, bool srgb)
{
	SKTX_header header;
	const uint8_t identifierReference[12] = KTX_IDENTIFIER_REF;
	memcpy(header.identifier, identifierReference, 12);
	header.endianness = KTX_ENDIAN_REF;
	header.glType = 0;
	header.glTypeSize = 1;
	header.glFormat = 0;
	header.glInternalFormat = srgb ? GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM : GL_COMPRESSED_RGBA_BPTC_UNORM;
	header.glBaseInternalFormat = GL_RGBA;
	header.pixelWidth = width;
	header.pixelHeight = height;
	header.pixelDepth = 0;
	header.numberOfArrayElements = 0;
	header.numberOfFaces = 1;
	header.numberOfMipmapLevels = 1;
	header.bytesOfKeyValueData = 0;

	fwrite(&header, sizeof(SKTX_header), 1, pFile);

	// Calculate number of blocks
	uint32_t blocks_x = (width + 3) / 4;
	uint32_t blocks_y = (height + 3) / 4;
	uint32_t image_size = blocks_x * blocks_y * sizeof(bc7_block);

	// Write image size for mip 0
	fwrite(&image_size, sizeof(uint32_t), 1, pFile);

	// Write image data
	fwrite(pBlocks, image_size, 1, pFile);

	// KTX requires each mip level to be 4-byte aligned
	uint32_t padding = (3 - ((image_size + 3) % 4));
	if (padding && padding < 4) 
	{
		static const uint8_t pad[3] = { 0, 0, 0 };
		fwrite(pad, padding, 1, pFile);
	}
}

void save_bc7_dds(FILE* pFile, uint32_t width, uint32_t height, const bc7_block* pBlocks, bool srgb)
{
	fwrite("DDS ", 4, 1, pFile);

	DDSURFACEDESC2 desc;
	memset(&desc, 0, sizeof(desc));

	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT | DDSD_CAPS;

	desc.dwWidth = width;
	desc.dwHeight = height;

	desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
	desc.ddpfPixelFormat.dwSize = sizeof(desc.ddpfPixelFormat);

	desc.ddpfPixelFormat.dwFlags |= DDPF_FOURCC;

	desc.ddpfPixelFormat.dwFourCC = (uint32_t)PIXEL_FMT_FOURCC('D', 'X', '1', '0');
	desc.ddpfPixelFormat.dwRGBBitCount = 0;

	const uint32_t pixel_format_bpp = 8;

	desc.lPitch = (((desc.dwWidth + 3) & ~3) * ((desc.dwHeight + 3) & ~3) * pixel_format_bpp) >> 3;
	desc.dwFlags |= DDSD_LINEARSIZE;

	fwrite(&desc, sizeof(desc), 1, pFile);

	DDS_HEADER_DXT10 hdr10;
	memset(&hdr10, 0, sizeof(hdr10));

	// Not all tools support DXGI_FORMAT_BC7_UNORM_SRGB (like NVTT), but ddsview in DirectXTex pays attention to it. So not sure what to do here.
	// For best compatibility just write DXGI_FORMAT_BC7_UNORM.
	hdr10.dxgiFormat = srgb ? DXGI_FORMAT_BC7_UNORM_SRGB : DXGI_FORMAT_BC7_UNORM;
	hdr10.resourceDimension = D3D10_RESOURCE_DIMENSION_TEXTURE2D;
	hdr10.arraySize = 1;

	fwrite(&hdr10, sizeof(hdr10), 1, pFile);

	fwrite(pBlocks, desc.lPitch, 1, pFile);
}


bool save_bc7(const char* pFilename, uint32_t width, uint32_t height, const bc7_block* pBlocks, bool srgb)
{
	const char* ext = get_file_extension(pFilename);
	bool isKTX = strcmp(ext, "ktx") == 0;
	bool isDDS = strcmp(ext, "dds") == 0;

	if (isKTX || isDDS)
	{
		FILE* pFile = NULL;
		pFile = fopen(pFilename, "wb");
		if (!pFile)
		{
			fprintf(stderr, "Failed creating file %s!\n", pFilename);
			return false;
		}

		if (isKTX)
		{
			save_bc7_ktx(pFile, width, height, pBlocks, srgb);
		}
		else if (isDDS)
		{
			save_bc7_dds(pFile, width, height, pBlocks, srgb);
		}

		if (fclose(pFile) == EOF)
		{
			fprintf(stderr, "Failed writing to file %s!\n", pFilename);
			return false;
		}
	}
	else
	{
		fprintf(stderr, "Unsupported texture container %s!\n", ext);
		return false;
	}

	return true;
}