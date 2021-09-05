/* 

Amine Rehioui
Created: April 22nd 2012

*/

#pragma once

#include <string>

class PNGLoader {
public:

	static void* Load(const std::string& path, int& width, int& height, int& channels);
};
