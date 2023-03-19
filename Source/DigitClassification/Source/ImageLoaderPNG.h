#pragma once

struct SPNGImage
{
	unsigned int Width;
	unsigned int Height;
	int BitDepth;
	int ColorType;
	int BPP;
	unsigned char* Data;
	unsigned int Pitch;

	SPNGImage()
	{
		Width = 0;
		Height = 0;
		BPP = 3;
		Data = NULL;
	}

	~SPNGImage()
	{
		if (Data)
			delete[]Data;
	}
};

bool loadPNG(const char* path, SPNGImage* image);