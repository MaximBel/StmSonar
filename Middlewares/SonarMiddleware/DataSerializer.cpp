/*
 * DataSerializer.cpp
 *
 *  Created on: 25 џэт. 2021 у.
 *      Author: dezzm
 */

#include <DataSerializer.h>
#include "vector"
#include <inttypes.h>

void SerialData::addKey(string key, string value) {
	dataMap->emplace(key, value);
}
shared_ptr<map<string, string>> SerialData::getMap() {
	return dataMap;
}

DataSerializer* DataSerializer::addDataMember(string dataKey,
		uint16_t& dataValue) {
	sprintf(tempDataString, "%" PRIu16 "", (unsigned) dataValue);
	serialData->addKey(dataKey, string(tempDataString));
	return this;
}

DataSerializer* DataSerializer::addDataMember(string dataKey,
		float& dataValue) {
	sprintf(tempDataString, "%f", dataValue);
	serialData->addKey(dataKey, string(tempDataString));
	return this;
}

DataSerializer* DataSerializer::addDataMember(string dataKey,
		string& dataValue) {
	serialData->addKey(dataKey, dataValue);
	return this;
}

DataSerializer* DataSerializer::addDataMember(string dataKey,
		vector<uint16_t>& dataValue) {
	uint32_t length = 0;

	length += sprintf(tempDataString + length, "[");
	for (auto element : dataValue) {
		length += sprintf(tempDataString + length, "%" PRIu16 ",",
				(unsigned) element);
	}
	sprintf(tempDataString + length, "]");
	serialData->addKey(dataKey, string(tempDataString));
	return this;
}

DataSerializer* DataSerializer::addDataMember(string dataKey,
		vector<float>& dataValue) {
	uint32_t length = 0;

	length += sprintf(tempDataString + length, "[");
	for (auto element : dataValue) {
		length += sprintf(tempDataString + length, "%f,", element);
	}
	sprintf(tempDataString + length, "]");
	serialData->addKey(dataKey, string(tempDataString));
	return this;
}

DataSerializer* DataSerializer::addDataMember(string dataKey,
		vector<string>& dataValue) {
	uint32_t length = 0;

	length += sprintf(tempDataString + length, "[");
	for (auto element : dataValue) {
		length += sprintf(tempDataString + length, "\"%s\",", element.c_str());
	}
	sprintf(tempDataString + length, "]");
	serialData->addKey(dataKey, string(tempDataString));
	return this;
}

string DataSerializer::serializeData() {
	auto dataMap = serialData->getMap();
	string outputString = "";

	for (auto const& element : *dataMap) {
		outputString += "\"" + element.first + "\":" + element.second + ",";
	}

	outputString = "{" + outputString + "}";

	return outputString;
}
