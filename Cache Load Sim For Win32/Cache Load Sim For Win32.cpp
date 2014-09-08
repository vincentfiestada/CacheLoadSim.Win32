// Cache Load Sim For Win32.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Cache.h"
#include <iostream>

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	// Initialize
	vector<DataChunk> MainMemory;
	int size = 0;
	int mapalgo = 1;
	int repalgo = 0;
	int mainmemsize = 0;
	unsigned int addresstoload = 0;
	int tempBuff = 0;

	// Collect simulation settings from user
	cout << "Cache Load Simulator (Win32)" << endl;
	cout << "(c) 2014 Vincent Fiestada" << endl;
	cout << "See README.txt for more info" << endl;
	cout << "+++++++++++++++++++++++++++++++++++++" << endl;
	cout << "SIMULATION SETTINGS:" << endl;
	cout << "Cache size: (2, 4 or 8)";
	cin >> size;
	// assert
	if (size != 2 && size != 4 && size != 8)
	{
		cout << "ERROR! Cache Size must be either 2, 4 or 8." << endl << "Enter anything to exit simulation" << endl;
		cin >> tempBuff;
		return 1;
	}
	cout << endl << "Mapping Algorithm ( 0 - Direct, 1 - Set Assoc, 2 - Full Assoc )" << endl;
	cout << "Enter choice: ";
	cin >> mapalgo;
	// assert
	if (mapalgo < 0 || mapalgo > 2)
	{
		cout << "ERROR! Mapping algorithm index out of range" << endl << "Enter anything to exit simulation" << endl;
		cin >> tempBuff;
		return 2;
	}
	cout << endl << "Replacement Algorithm (0 - FIFO, 1 - LRU, 2 - LFU )" << endl;
	cout << "Enter choice: ";
	cin >> repalgo;
	// assert
	if (repalgo < 0 || repalgo > 2)
	{
		cout << "ERROR! Replacement algorithm index out of range" << endl << "Enter anything to exit simulation" << endl;
		cin >> tempBuff;
		return 3;
	}
	
	mainmemsize = size*size;
	cout << "Main Memory size is " << mainmemsize << endl;
	cout << "You can load addresses from 0 to " << mainmemsize - 1 << endl;

	Cache ProcessorCache(size, mapalgo, repalgo, &MainMemory);

	int dataToPut = 0x00;
	// Populate Main Memory
	for (int i = 0; i < 16; i++)
	{
		MainMemory.push_back(DataChunk(i, dataToPut));
		dataToPut += 0x11;
	}

	while (addresstoload != -1)
	{
		cout << "Enter address to load ( 0 - " << mainmemsize - 1 << ")" << endl << "Enter -1 to exit" << endl;
		cin >> addresstoload;

		// Check for out of range address
		if (addresstoload >= MainMemory.size())
		{
			cout << "Address is out of range" << endl;
			continue;
		}
		else if (addresstoload < 0)
		{
			break;
		}

		ProcessorCache.loadAddress(addresstoload);
		cout << "=======================================" << endl;
		cout << "Cache Slots: " << endl;
		for (int i = 0; i < size; i++)
		{
			cout << i << " : " << "ref address ";
			auto addressToShow = ProcessorCache.getAtSlot(i)->Address;
			auto dataToShow = ProcessorCache.getAtSlot(i)->Data;
			if (addressToShow < 0)
			{
				cout << "NOTHING";
			}
			else
			{
				cout << addressToShow;
			}
			cout << " : contains ";
			if (dataToShow < 0)
			{
				cout << "NOTHING";
			}
			else
			{
				cout << dataToShow;
			}
			cout << endl;
		}
		cout << "---------------------------------------" << endl;
		cout << "TOTAL HITS: " << ProcessorCache.getTotalHits() << endl;
		cout << "TOTAL HIT TIME: " << ProcessorCache.getHitTime() << endl;
		cout << "TOTAL MISSES: " << ProcessorCache.getTotalMisses() << endl;
		cout << "TOTAL MISS PENALTY: " << ProcessorCache.getTotalMisses() << endl;
		cout << "=======================================" << endl;
	}

	return 0;
}