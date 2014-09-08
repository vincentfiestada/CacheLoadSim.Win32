#include "stdafx.h"
#include "Cache.h"

#define NOTHING -1

Cache::Cache(int size, int map, int replace, vector<DataChunk>* RAMTable)
{
	// size		   = number of cache entries
	// map         = mapping algorithm to use
	// replace     = replacement algorithm to use
	this->_size = size;
	for (int i = 0; i < size; i++)
	{
		auto newchunk = &DataChunk(NOTHING, 0);
		this->_entries.push_back(newchunk);
	}
	this->_RAMTable = RAMTable;
	this->_mapping = map;
	this->_replacement = replace;
	this->_lastFIFOTracker = 0;
	this->_totalHits = 0;
	this->_totalMisses = 0;
	this->_totalHitTime = 0;
	this->_totalMissPenalty = 0;
	// get the number of bitshifts to use when getting the index for direct and set assoc mapping
	// 1) Get an index Template, of there are (in binary) 000 - 111 slots, you need 3 bits as index
	//    If we take those three bits from the rightmost part of the address, we can just use
	//    a bitwise  AND (&) with the address and the number of slots - 1
	int bitcount = 0;
	this->_mappingIndexTemplate = 0;
	if (this->_mapping == Direct)
	{
		bitcount = floor(log2(size));
		for (int i = 0; i < bitcount; i++)
		{
			this->_mappingIndexTemplate <<= 1;
			this->_mappingIndexTemplate += 1;
		}
		
	}
	else if (this->_mapping == Set)
	{
		// Number of bits to use for set assoc mapping is always log(base:2 of n) - 1
		bitcount = floor(log2(size)) - 1;
		for (int i = 0; i < bitcount; i++)
		{
			this->_mappingIndexTemplate <<= 1;
			this->_mappingIndexTemplate += 1;
		}
	}
}

void Cache::loadAddress(int address)
{
	// We can treat all three mapping algorithms as set associative
	//		Set Associative maps addresses to 2 consecutive cache entries
	//		Direct mapping maps addresses to 1 cache entry
	//		Fully Associative maps addresses to all cache entries
	unsigned int mapStart = 0;
	unsigned int mapEnd = 0;
	int tempAddress = address;

	switch (this->_mapping)
	{
	case Direct:
		tempAddress &= this->_mappingIndexTemplate;
		mapStart = tempAddress;
		mapEnd = mapStart;
		break;
	case Set:
		if (this->_mappingIndexTemplate == 0)
		{
			tempAddress &= 1;
			mapStart = tempAddress;
			mapEnd = mapStart;
		}
		else
		{
			tempAddress &= this->_mappingIndexTemplate;
			mapStart = tempAddress * 2;
			mapEnd = mapStart + 1;
		}
		break;
	case Full:
		mapStart = 0;
		mapEnd = this->_size - 1;
		break;
	default:
		throw new exception("Mapping algorithm is not recognized");
	}

	// Now that we know the start and end of the map, look for the data chunk w/ same address in the cache
	for (unsigned int i = mapStart; i <= mapEnd; i++)
	{
		this->incrementHitTime(); // each time we look at a slot, hit time increases
		if (this->_entries[i]->Address == address)
		{
			this->incrementTotalHits();
			if (this->_replacement == LRU || this->_replacement == LFU)
			{
				this->_entries[i]->AccessFrequency++;
			}
			return; // data found; return
		}
	}

	// Load function did not return, so the data is not in the cache yet
	//      First, increment total miss count...and then
	//		we need to load it from Main Memory
	this->incrementTotalMisses();

	// Find an empty slot in the 'map'
	for (unsigned int i = mapStart; i <= mapEnd; i++)
	{
		if (i < 0 || i > _size - 1) continue;
		// each time we look for an empty slot, increment total miss penalty
		this->incrementTotalMissPenalty();
		if (this->_entries[i]->Address == NOTHING) // If address is the constant NOTHING, the slot is empty
		{
			// Now we can load the data chunk from RAM
			auto dataChunkX = this->_RAMTable->at(address);
			this->_entries[i] = &dataChunkX;
			if (this->_replacement == LRU || this->_replacement == LFU)
			{
				this->_lastFIFOTracker += 1;
				this->_entries[i]->AccessFrequency = this->_lastFIFOTracker;
				this->_entries[i]->AccessFrequency++;
			}
			else if (_replacement == FIFO)
			{
				this->_entries[i]->AccessFrequency = this->_lastFIFOTracker + 1;
				this->_lastFIFOTracker++;
			}
			return; // Empty slot found; data chunk loaded; return from function
		}
	}

	// No empty slot was found, so we need to choose a slot in the map to replace

	int min = -1;
	int indexOfSlotToReplace = -1;

	// Whatever replacement algorithm is used, just replace the slot which has the lowest access frequency
	//		If using FIFO, the frequency of each new loaded data chunk is 1 + the highest previous frequency
	//			And it is reset when the chunk is unloaded
	//		If LRU, the frequency is incremented each time a chunk is used
	//			And it is reset when the chunk is unloaded
	//		If LFU, the frequency is incremented each time a chunk is used
	//			But it is never reset
	//		Note: LRU & LFU still incorporate the lastFIFOTracker so that there is no bias on positioning

	for (unsigned int i = mapStart; i <= mapEnd; i++)
	{
		if (this->_entries[i]->AccessFrequency < min || min == -1)
		{
			min = this->_entries[i]->AccessFrequency;
			indexOfSlotToReplace = i;
		}
	}

	// If FIFO or LRU, reset access frequency counter
	if (this->_replacement == FIFO || this->_replacement == LRU)
	{
		this->_entries[indexOfSlotToReplace]->AccessFrequency = 0;
	}
	this->_entries[indexOfSlotToReplace] = &(this->_RAMTable->at(address));
	// Now take care of the newly loaded chunk's frequency counter
	if (this->_replacement == LRU || this->_replacement == LFU)
	{
		this->_lastFIFOTracker += 1;
		this->_entries[indexOfSlotToReplace]->AccessFrequency = this->_lastFIFOTracker;
		this->_entries[indexOfSlotToReplace]->AccessFrequency += 1;
	}
	else if (this->_replacement == FIFO)
	{
		this->_lastFIFOTracker += 1;
		this->_entries[indexOfSlotToReplace]->AccessFrequency = this->_lastFIFOTracker;
	}
	
	return;

}