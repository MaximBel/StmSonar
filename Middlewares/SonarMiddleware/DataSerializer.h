/*
 * DataSerializer.h
 *
 *  Created on: 25 џэт. 2021 у.
 *      Author: dezzm
 */

#ifndef SONARMIDDLEWARE_DATASERIALIZER_H_
#define SONARMIDDLEWARE_DATASERIALIZER_H_

#include <string>
#include "map"
#include "memory"
#include "vector"

using namespace std;

class SerialData {
public:
	SerialData() :
			dataMap(make_shared<map<string, string>>()) {
	}
	virtual ~SerialData() = default;

	void addKey(string key, string value);
	shared_ptr<map<string, string>> getMap();
private:
	shared_ptr<map<string, string>> dataMap;
};

class DataSerializer {
public:
	DataSerializer() :
			serialData(unique_ptr<SerialData>(new SerialData())) {
	}
	virtual ~DataSerializer() = default;

	// overloading is better, don't want to use templates due to complexity of
	// related solutions. could not use to_string, maybe, unsupported.
	DataSerializer* addDataMember(string dataKey, uint64_t& dataValue);
	DataSerializer* addDataMember(string dataKey, float& dataValue);
	DataSerializer* addDataMember(string dataKey, string& dataValue);
	DataSerializer* addDataMember(string dataKey, vector<uint64_t>& dataValue);
	DataSerializer* addDataMember(string dataKey, vector<float>& dataValue);
	DataSerializer* addDataMember(string dataKey, vector<string>& dataValue);

	string serializeData();

private:
	unique_ptr<SerialData> serialData;
	char tempDataString[500];
};

#endif /* SONARMIDDLEWARE_DATASERIALIZER_H_ */
