// PolyNova3D (version 3.3)
/***************************************************************
 Copyright (C) 1999 Novasoft Consulting

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Library General Public
 License as published by the Free Software Foundation; either
 version 2 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Library General Public License for more details.

 You should have received a copy of the GNU Library General Public
 License along with this library; if not, write to the Free
 Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************/
#include "poly_nova.h"

Yamlish::Yamlish() {
	// Empty;
}

Yamlish::~Yamlish() {
	globals.clear();
	sections.clear();
}

static std::string trim(const std::string &str) {
	size_t start = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");
	return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

bool Yamlish::load(const std::string &filename) {
	FILE *file = fopen(filename.c_str(), "r");
	if (!file) {
		return false;
	}

	globals.clear();
	sections.clear();

	Section *currentSection = NULL;
	char tempLine[1024];
	std::string line, currentListKey;

	while (fgets(tempLine, sizeof(tempLine), file)) {
		line = tempLine;
		line = trim(line);
		if (line.empty() || line[0] == '#') {
			continue;
		}

		if (line.back() == ':' && line.find(':') == line.size() - 1) {
			line.pop_back();
			line = trim(line);
			sections.push_back( { line, { } });
			currentSection = &sections.back();
			currentListKey.clear();
			continue;
		}

		if (line[0] == '-') {
			if (!currentListKey.empty() && currentSection) {
				for (auto &kv : currentSection->items) {
					if (kv.key == currentListKey) {
						std::string item = trim(line.substr(1));
						if (!kv.value.empty())
							kv.value += ",";
						kv.value += item;
						break;
					}
				}
			}
			continue;
		}

		auto pos = line.find(':');
		if (pos == std::string::npos) {
			continue;
		}

		std::string key = trim(line.substr(0, pos));
		std::string value = trim(line.substr(pos + 1));

		KeyValue kv { key, value, isInlineList(value) };
		if (kv.isList) {
			if (value.front() == '[' && value.back() == ']') {
				kv.value = trim(value.substr(1, value.size() - 2));
			}
		} else if (value.empty()) {
			currentListKey = key;
			kv.isList = true;
		}

		if (currentSection) {
			currentSection->items.push_back(kv);
		} else {
			globals.push_back(kv);
		}
	}

	fclose(file);
	return true;
}

bool Yamlish::save(const std::string &filename) const {
	FILE *file = fopen(filename.c_str(), "w");
	if (!file) {
		return false;
	}

	for (const auto &kv : globals) {
		if (kv.isList)
			fprintf(file, "%s: [ %s ]\n", kv.key.c_str(), kv.value.c_str());
		else
			fprintf(file, "%s: %s\n", kv.key.c_str(), kv.value.c_str());
	}

	if (!globals.empty()) {
		fprintf(file, "\n");
	}

	for (const auto &sec : sections) {
		fprintf(file, "%s:\n", sec.name.c_str());

		for (const auto &kv : sec.items) {
			if (kv.isList)
				fprintf(file, "  %s: [ %s ]\n", kv.key.c_str(), kv.value.c_str());
			else
				fprintf(file, "  %s: %s\n", kv.key.c_str(), kv.value.c_str());
		}

		fprintf(file, "\n");
	}

	fclose(file);
	return true;
}

const Yamlish::KeyValue* Yamlish::findItem(const std::string &fullKey) const {
	auto dot = fullKey.find('.');
	if (dot == std::string::npos) {
		for (const auto &kv : globals) {
			if (kv.key == fullKey)
				return &kv;
		}
	} else {
		std::string section = fullKey.substr(0, dot);
		std::string key = fullKey.substr(dot + 1);
		for (const auto &sec : sections) {
			if (sec.name == section) {
				for (const auto &kv : sec.items) {
					if (kv.key == key)
						return &kv;
				}
			}
		}
	}
	return NULL;
}

Yamlish::KeyValue* Yamlish::findOrCreateItem(const std::string &fullKey) {
	auto dot = fullKey.find('.');
	if (dot == std::string::npos) {
		for (auto &kv : globals) {
			if (kv.key == fullKey)
				return &kv;
		}
		globals.push_back( { fullKey, "", false });
		return &globals.back();
	} else {
		std::string section = fullKey.substr(0, dot);
		std::string key = fullKey.substr(dot + 1);
		for (auto &sec : sections) {
			if (sec.name == section) {
				for (auto &kv : sec.items) {
					if (kv.key == key)
						return &kv;
				}
				sec.items.push_back( { key, "", false });
				return &sec.items.back();
			}
		}
		sections.push_back( { section, { { key, "", false } } });
		return &sections.back().items.front();
	}
}

std::string Yamlish::get(const std::string &key) const {
	const KeyValue *kv = findItem(key);
	return kv ? kv->value : "";
}

int Yamlish::getInt(const std::string &key, int defaultValue) const {
	std::string val = get(key);
	return val.empty() ? defaultValue : std::atoi(val.c_str());
}

float Yamlish::getFloat(const std::string &key, float defaultValue) const {
	std::string val = get(key);
	return val.empty() ? defaultValue : std::atof(val.c_str());
}

bool Yamlish::getBool(const std::string &key, bool defaultValue) const {
	std::string val = get(key);
	std::string lval = val;
	for (char &c : lval) {
		if (c >= 'A' && c <= 'Z')
			c += 'a' - 'A';
	}

    if (lval == "true" || lval == "yes" || lval == "on" || lval == "y" || lval == "1") {
    	return true;
    }

    if (lval == "false" || lval == "no" || lval == "off" || lval == "n" || lval == "0") {
    	return false;
    }

	return defaultValue;
}

std::vector<std::string> Yamlish::splitList(const std::string &value) {
	std::vector<std::string> result;
	std::string item;
	size_t start = 0, end;
	while ((end = value.find(',', start)) != std::string::npos) {
		result.push_back(trim(value.substr(start, end - start)));
		start = end + 1;
	}
	result.push_back(trim(value.substr(start)));
	return result;
}

std::vector<std::string> Yamlish::getList(const std::string &key) const {
	const KeyValue *kv = findItem(key);
	return kv && kv->isList ? splitList(kv->value) : std::vector<std::string> { };
}

std::vector<int> Yamlish::getIntList(const std::string& key) const {
    std::vector<std::string> stringList = getList(key);
    std::vector<int> intList;
    for (const std::string& item : stringList) {
        intList.push_back(std::atoi(item.c_str()));
    }
    return intList;
}

std::vector<float> Yamlish::getFloatList(const std::string& key) const {
    std::vector<std::string> stringList = getList(key);
    std::vector<float> floatList;
    for (const std::string& item : stringList) {
        floatList.push_back(std::atof(item.c_str()));
    }
    return floatList;
}

std::vector<bool> Yamlish::getBoolList(const std::string& key) const {
    std::vector<std::string> stringList = getList(key);
    std::vector<bool> boolList;
    for (std::string item : stringList) {
        // Normalize to lowercase
        for (char& c : item) {
            if (c >= 'A' && c <= 'Z') {
                c += 'a' - 'A';
            }
        }
        if (item == "true" || item == "yes" || item == "on" || item == "y" || item == "1") {
            boolList.push_back(true);
        } else if (item == "false" || item == "no" || item == "off" || item == "n" || item == "0") {
            boolList.push_back(false);
        } else {
            // Unknown string -> Default to false (or you could throw an error/log warning if you want)
            boolList.push_back(false);
        }
    }
    return boolList;
}

bool Yamlish::set(const std::string &key, const std::string &value) {
	KeyValue *kv = findOrCreateItem(key);
	kv->value = value;
	kv->isList = isInlineList(value);
	if (kv->isList && value.front() == '[' && value.back() == ']') {
		kv->value = trim(value.substr(1, value.size() - 2));
	}
	return true;
}

bool Yamlish::setInt(const std::string &key, int value) {
	return set(key, std::to_string(value));
}

bool Yamlish::setFloat(const std::string &key, float value) {
	return set(key, std::to_string(value));
}

bool Yamlish::setBool(const std::string &key, bool value) {
	return set(key, value ? "true" : "false");
}

bool Yamlish::isInlineList(const std::string &val) {
	return val.size() > 1 && val.front() == '[' && val.back() == ']';
}
