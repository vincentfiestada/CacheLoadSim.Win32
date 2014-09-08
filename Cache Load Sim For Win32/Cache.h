#pragma once

#include "DataChunk.h"
#include <vector>

#define Direct 0
#define Set 1
#define Full 2
#define FIFO 0
#define LRU 1
#define LFU 2

using namespace std;

class Cache
{
public:
	Cache(int size, int map, int replace, vector<DataChunk>* RAMTable);

	void loadAddress(int address);
	void setSize(int size)
	{
		this->_size = size;
	}
	int getSize()
	{
		return this->_size;
	}
	std::wstring getMappingAlgorithm()
	{
		switch (this->_mapping)
		{
		case Direct:
			return L"Direct Mapping Algorithm";
			break;
		case Full:
			return L"Fully Associative Mapping Algorithm";
			break;
		case Set:
			return L"Set Associative Mapping Algorithm";
			break;
		default:
			return L"Unknown Mapping Algorithm [Beware of iminent exceptions]";
		}
	}
	std::wstring getReplacementAlgorithm()
	{
		switch (this->_replacement)
		{
		case FIFO:
			return L"First-in-First-Out";
			break;
		case LRU:
			return L"Least Recently Used is replaced";
			break;
		case LFU:
			return L"Least Frequently Used is replaced";
			break;
		default:
			return L"Random Slot is replaced";
		}
	}
	void setTotalHits(int x)
	{
		this->_totalHits = x;
	}
	int getTotalHits() 
	{ 
		return this->_totalHits;
	}
	void incrementTotalHits()
	{
		this->_totalHits++;
	}
	void setTotalMisses(int x)
	{
		this->_totalMisses = x;
	}
	int getTotalMisses() 
	{ 
		return this->_totalMisses;
	}
	void incrementTotalMisses()
	{
		this->_totalMisses++;
	}
	void setHitTime(int x)
	{
		this->_totalHitTime = x;
	}
	int getHitTime() 
	{ 
		return this->_totalHitTime;
	}
	void incrementHitTime()
	{
		this->_totalHitTime++;
	}
	void setTotalMissPenalty(int x)
	{
		this->_totalMissPenalty = x;
	}
	int getTotalMissPenalty() 
	{ 
		return this->_totalMissPenalty;
	}
	void incrementTotalMissPenalty()
	{
		this->_totalMissPenalty++;
	}
	DataChunk* getAtSlot(int index)
	{
		return _entries.at(index);
	}

private:
	int _size;
	int _mappingIndexTemplate;
	vector<DataChunk> *_RAMTable;
	vector<DataChunk*> _entries;
	int _lastFIFOTracker;
	int _mapping;
	int _replacement;
	// To estimate performance, monitor hit rate through total number of Hits
	//      , miss rate through total number of misses
	//		, total hit time [ includes time it takes to find the data chunk; still counted if it is a miss ]
	//		, total miss penalty [ includes time it takes to find which slot to replace or load into ]
	int _totalHitTime;
	int _totalHits;
	int _totalMissPenalty;
	int _totalMisses;
	
};