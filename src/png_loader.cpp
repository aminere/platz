
#include "pch.h"
#include "png_loader.h"
#include <assert.h>

#include "png.h"
#include "pnginfo.h"

namespace platz {

	namespace pngloader {
		png_voidp png_malloc(png_structp png_ptr, png_size_t size) {
			return new unsigned char[size];
		}

		//! PNG free
		void png_free(png_structp png_ptr, png_voidp ptr) {
			delete[](unsigned char*)ptr;
		}

		//! PNG read
		void png_read(png_structp png_ptr, png_bytep data, png_size_t length) {
			auto fp = (FILE*)png_get_io_ptr(png_ptr);
			fread(data, 1, length, fp);
		}
	}

	unsigned char* PNGLoader::load(const std::string& path, int& width, int& height, int& channels) {
		auto fp = fopen(path.c_str(), "rb");
		if (!fp) {
			return NULL;
		}

		png_structp png_ptr = png_create_read_struct_2(PNG_LIBPNG_VER_STRING,
			/*error_ptr*/NULL,
			/*error_ptr*/NULL,
			/*warning_ptr*/NULL,
			/*mem_ptr*/NULL,
			pngloader::png_malloc,
			pngloader::png_free);

		assert(png_ptr);

		png_infop info_ptr = png_create_info_struct(png_ptr);
		assert(info_ptr);

		png_set_read_fn(png_ptr, fp, pngloader::png_read);

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
		default: assert(false);
		}

		auto row_bytes = png_get_rowbytes(png_ptr, info_ptr);

		assert(row_bytes == width * channels);
		png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

		auto pngData = new unsigned char[row_bytes * height];
		for (int i = 0; i < height; ++i) {
			memcpy(pngData + (row_bytes * i), row_pointers[i], row_bytes);
		}

		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

		fclose(fp);
		return pngData;
	}
}

