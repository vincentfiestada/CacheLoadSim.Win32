#pragma once
#include "stdafx.h"

class DataChunk
{
public:
	DataChunk(int address, int data)
	{
		Address = address;
		Data = data;
		AccessFrequency = 0;
	}

	int Address;
	int Data;
	int AccessFrequency;
};