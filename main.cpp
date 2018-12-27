#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <numeric>
#include <limits.h>
#include <cmath>
#include <iostream>
#include "fineGrainedSync.h"
#include "lazySync.h"

#define WRITERS_NUMB 5
#define READERS_NUMB 5

using namespace std;

struct Args {
	int dataSize;
	vector<int> data;
	mySet<int> * set;
};

void *AddElem(void * args)
{
	Args *arguments = (Args*)args;

	for (int i = 0; i < arguments->dataSize; i++)
	{
		arguments->set->Add(arguments->data[i]);
	}
	pthread_exit(NULL);
	return NULL;
}


void *DeleteElem(void * args)
{
	Args* arguments = (Args*)args;
	for (int i = 0; i < arguments->dataSize; i++)
	{
		arguments->set->Remove(arguments->data[i]);
	}
	pthread_exit(NULL);
	return NULL;
}


bool runTest(mySet<int> * set, int readersNum, int writersNum)
{
	bool writersTest = true, readersTest = true;
	vector <pthread_t> writers(writersNum);
	vector <pthread_t> readers(readersNum);
	vector<int> addedData;  //size 3*writers

	cout << "starting writers test\n";
	for (int i = 0; i < writersNum; i++)
	{
		vector<int> dataToAdd;
		addedData.push_back(3 * i);
		addedData.push_back(3 * i + 1);
		addedData.push_back(3 * i + 2);

		dataToAdd.push_back(3 * i);
		dataToAdd.push_back(3 * i + 1);
		dataToAdd.push_back(3 * i + 2);

		Args *argsStruct;
		argsStruct = new Args;

		argsStruct->dataSize = 3;
		argsStruct->data = dataToAdd;
		argsStruct->set = set;

		pthread_create(&writers[i], NULL, AddElem, (void *)argsStruct);
		dataToAdd.resize(0);
	}

	for (int i = 0; i < writersNum; i++)
	{
		pthread_join(writers[i], NULL);
	}

	for (unsigned int i = 0; i < addedData.size(); i++)
	{
		if (!set->Contains(addedData[i])) {
			writersTest = false;
			break;
		}
	}

	if (writersTest)
		cout << "writers test: ok\n";
	else
		cout << "writers test: failed\n";

	cout << "starting readers test\n";

	int curpos = 0;
	for (int i = 0; i < readersNum; i++)
	{
		vector<int> dataToRemove;
		for (int j = 0; j < ceil(addedData.size() / readersNum); j++)
		{
			dataToRemove.push_back(addedData[curpos]);
			curpos ++;
		}
		Args *argsStruct;
		argsStruct = new Args;

		argsStruct->dataSize = 3;
		argsStruct->data = dataToRemove;
		argsStruct->set = set;

		pthread_create(&readers[i], NULL, DeleteElem, (void *)argsStruct);
		dataToRemove.resize(0);
	}

	for (int i = 0; i < readersNum; i++)
	{
		pthread_join(readers[i], NULL);
	}

	for (unsigned int i = 0; i < addedData.size(); i++)
	{
		if (set->Contains(addedData[i])) {
			readersTest = false;
			break;
		}
	}

	if (readersTest)
		cout << "readers test: ok\n";
	else
		cout << "readers test: failed\n";

	return true;
}

int main (int argc, char * argv [])
{
	mySet<int> *set;
	set = new FGSet<int>(INT_MIN, INT_MAX);

	cout << "TEST FOR FINE GRAINDES SET\n";
	runTest(set, READERS_NUMB, WRITERS_NUMB);
	cout << "TEST END\n\n";

	delete set;

	set = new LSSet<int>(INT_MIN, INT_MAX);

	cout << "TEST FOR LAZY SET\n";
	runTest(set, READERS_NUMB, WRITERS_NUMB);
	cout << "TEST END\n\n";

	delete set;

	return 0;
}
