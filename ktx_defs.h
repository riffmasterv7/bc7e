#pragma once

#include <cstdint>

#define KTX_IDENTIFIER_REF  { 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A }
#define KTX_ENDIAN_REF      (0x04030201)

struct SKTX_header
{
	uint8_t  identifier[12];

	uint32_t endianness;

	/**
	* @brief The type of the image data.
	*
	* Values are the same as in the @p type parameter of
	* glTexImage*D. Must be 0 for compressed images.
	*/
	uint32_t glType;
	/**
	* @brief The data type size to be used in case of endianness
	*        conversion.
	*
	* This value is used in the event conversion is required when the
	* KTX file is loaded. It should be the size in bytes corresponding
	* to glType. Must be 1 for compressed images.
	*/
	uint32_t glTypeSize;
	/**
	* @brief The format of the image(s).
	*
	* Values are the same as in the format parameter
	* of glTexImage*D. Must be 0 for compressed images.
	*/
	uint32_t glFormat;
	/** @brief The internalformat of the image(s).
	*
	* Values are the same as for the internalformat parameter of
	* glTexImage*2D. Note: it will not be used when a KTX file
	* containing an uncompressed texture is loaded into OpenGL ES.
	*/
	uint32_t glInternalFormat;
	/** @brief The base internalformat of the image(s)
	*
	* For non-compressed textures, should be the same as glFormat.
	* For compressed textures specifies the base internal, e.g.
	* GL_RGB, GL_RGBA.
	*/
	uint32_t glBaseInternalFormat;

	/** @brief Width of the image for texture level 0, in pixels. */
	uint32_t pixelWidth;
	/** @brief Height of the texture image for level 0, in pixels.
	*
	* Must be 0 for 1D textures.
	*/
	uint32_t pixelHeight;
	/** @brief Depth of the texture image for level 0, in pixels.
	*
	* Must be 0 for 1D, 2D and cube textures.
	*/
	uint32_t pixelDepth;
	/** @brief The number of array elements.
	*
	* Must be 0 if not an array texture.
	*/
	uint32_t numberOfArrayElements;
	/** @brief The number of cubemap faces.
	*
	* Must be 6 for cubemaps and cubemap arrays, 1 otherwise. Cubemap
	* faces must be provided in the order: +X, -X, +Y, -Y, +Z, -Z.
	*/
	uint32_t numberOfFaces;
	/** @brief The number of mipmap levels.
	*
	* 1 for non-mipmapped texture. 0 indicates that a full mipmap pyramid should
	* be generated from level 0 at load time (this is usually not allowed for
	* compressed formats). Mipmaps must be provided in order from largest size to
	* smallest size. The first mipmap level is always level 0.
	*/
	uint32_t numberOfMipmapLevels;

	uint32_t bytesOfKeyValueData;
};

#define GL_RGBA 0x1908

/// BPTC formats
#define GL_COMPRESSED_RGBA_BPTC_UNORM                 0x8E8C
#define GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM           0x8E8D
#define GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT           0x8E8E
#define GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT         0x8E8F