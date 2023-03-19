#include "pch.h"
#include "ImageLoaderPNG.h"

#include <fstream>
#include <libpng/png.h>

static void png_cpexcept_error(png_structp png_ptr, png_const_charp msg)
{
	printf("PNG fatal error: %s", msg);
	longjmp(png_jmpbuf(png_ptr), 1);
}

// PNG function for warning handling
static void png_cpexcept_warn(png_structp png_ptr, png_const_charp msg)
{
	printf("PNG warning: %s", msg);
}

void PNGAPI user_read_data_fcn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	// changed by zola {
	std::ifstream* file = (std::ifstream*)png_get_io_ptr(png_ptr);

	try {
		file->read((char*)data, length);
	}
	catch (std::ios_base::failure& e)
	{
		printf(e.what());
		printf("\n");
		png_error(png_ptr, "Read Error");
	}
}

bool loadPNG(const char* path, SPNGImage* image)
{
	std::ifstream file;

	try {
		file.open(path, std::ios::binary);

		// Check if it really is a PNG file
		png_byte buffer[8];
		file.read((char*)buffer, 8);
		if (png_sig_cmp(buffer, 0, 8))
		{
			return false;
		}
	}
	catch (std::ios_base::failure& e)
	{
		printf(e.what());
		printf("\n");
		return false;
	}

	png_byte** rowPointers = NULL;

	// Allocate the png read struct
	png_structp png_ptr = png_create_read_struct(
		PNG_LIBPNG_VER_STRING,
		NULL,
		(png_error_ptr)png_cpexcept_error,
		(png_error_ptr)png_cpexcept_warn);
	if (!png_ptr)
	{
		return false;
	}

	// Allocate the png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return 0;
	}

	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		if (rowPointers)
			delete[] rowPointers;
		return 0;
	}

	// changed by zola so we don't need to have public FILE pointers
	png_set_read_fn(png_ptr, &file, user_read_data_fcn);

	png_set_sig_bytes(png_ptr, 8); // Tell png that we read the signature

	png_read_info(png_ptr, info_ptr); // Read the info section of the png file

	// Extract info
	png_get_IHDR(png_ptr, info_ptr,
		&image->Width,
		&image->Height,
		&image->BitDepth,
		&image->ColorType,
		NULL, NULL, NULL);

	// Convert low bit colors to 8 bit colors
	if (image->BitDepth < 8)
	{
		if (image->ColorType == PNG_COLOR_TYPE_GRAY || image->ColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
			png_set_expand_gray_1_2_4_to_8(png_ptr);
		else
			png_set_packing(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Convert high bit colors to 8 bit colors
	if (image->BitDepth == 16)
		png_set_strip_16(png_ptr);

	// Convert gray color to true color
	if (image->ColorType == PNG_COLOR_TYPE_GRAY || image->ColorType == PNG_COLOR_TYPE_GRAY_ALPHA)
		png_set_gray_to_rgb(png_ptr);

	int intent;
	const double screen_gamma = 2.2;

	if (png_get_sRGB(png_ptr, info_ptr, &intent))
		png_set_gamma(png_ptr, screen_gamma, 0.45455);
	else
	{
		double image_gamma;
		if (png_get_gAMA(png_ptr, info_ptr, &image_gamma))
			png_set_gamma(png_ptr, screen_gamma, image_gamma);
		else
			png_set_gamma(png_ptr, screen_gamma, 0.45455);
	}

	// Update the changes in between, as we need to get the new color type
	// for proper processing of the RGBA type
	png_read_update_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr,
		&image->Width,
		&image->Height,
		&image->BitDepth,
		&image->ColorType,
		NULL, NULL, NULL);

	rowPointers = new png_bytep[image->Height];
	if (!rowPointers)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		delete image;
		return false;
	}

	if (image->ColorType == PNG_COLOR_TYPE_RGB_ALPHA)
	{
		image->Data = new unsigned char[image->Width * image->Height * 4];
		image->Pitch = image->Width * 4;
	}
	else
	{
		image->Data = new unsigned char[image->Width * image->Height * 3];
		image->Pitch = image->Width * 3;
	}

	unsigned char* data = (unsigned char*)image->Data;
	for (unsigned int i = 0; i < image->Height; ++i)
	{
		rowPointers[i] = image->Data;
		data += image->Pitch;
	}

	// for proper error handling
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		delete[] rowPointers;
		return 0;
	}

	// Read data using the library function that handles all transformations including interlacing
	png_read_image(png_ptr, rowPointers);

	png_read_end(png_ptr, NULL);
	delete[] rowPointers;

	// Clean up memory
	png_destroy_read_struct(&png_ptr, &info_ptr, 0);
	return true;
}