/* 

Amine Rehioui
Created: April 22nd 2012

*/

#pragma once

#include <string>

namespace pngloader {
	unsigned char* load(const std::string& path, int& width, int& height, int& channels);
}
