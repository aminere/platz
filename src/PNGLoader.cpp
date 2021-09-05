/*

Amine Rehioui
Created: April 22nd 2012

*/

#include "PNGLoader.h"

#include "png.h"
#include "pnginfo.h"

png_voidp PNGMalloc(png_structp png_ptr, png_size_t size) {
	return new unsigned char[size];
}

//! PNG free
void PNGFree(png_structp png_ptr, png_voidp ptr) {
	delete[](unsigned char*)ptr;
}

//! PNG error
void PNGError(png_structp png_ptr, png_const_charp msg) {
	//SHOOT_ASSERT(false, msg);
}

//! PNG warning
void PNGWarning(png_structp png_ptr, png_const_charp msg) {
	//SHOOT_WARNING(false, msg);
}

//! PNG read
void PNGRead(png_structp png_ptr, png_bytep data, png_size_t length) {
	auto fp = (FILE*)png_get_io_ptr(png_ptr);
	fread(data, 1, length, fp);
	// pFile->Read(data, length);
}

void* PNGLoader::Load(const std::string& path, int& width, int& height, int& channels) {
	FILE* fp = fopen(path.c_str(), "rb");
	if (!fp) {
		return NULL;
	}	

	png_structp png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
		/*error_ptr*/NULL,
		PNGError,
		PNGWarning,
		/*mem_ptr*/NULL,
		PNGMalloc,
		PNGFree);

	//SHOOT_ASSERT(png_ptr, "png_create_read_struct_2 failed");

	png_infop info_ptr = png_create_info_struct(png_ptr);
	//SHOOT_ASSERT(info_ptr, "png_create_info_struct failed");

	png_set_read_fn(png_ptr, fp, PNGRead);

	int sig_read = 0;
	png_set_sig_bytes(png_ptr, sig_read);

	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	width = info_ptr->width;
	height = info_ptr->height;

	switch (info_ptr->color_type) {
	case PNG_COLOR_TYPE_RGBA:
		channels = 4;
		break;

	case PNG_COLOR_TYPE_RGB:
		channels = 3;
		break;
		//default: SHOOT_ASSERT(false, "Unsupported PNG format");
	}

	auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);

	//SHOOT_ASSERT(row_bytes == width * channels, "PNG row_bytes inconsistent with image format");
	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	auto pngData = new unsigned char[row_bytes * height];
	for (int i = 0; i < height; ++i) {
		memcpy(pngData + (row_bytes * i), row_pointers[i], row_bytes);
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);	
	return pngData;
}
