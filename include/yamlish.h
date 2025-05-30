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
#ifndef __YAMLISH_H
#define __YAMLISH_H

#include <string>

class Yamlish {
public:
    Yamlish();
    ~Yamlish();

    bool load(const std::string& filename);
    bool save(const std::string& filename) const;

    std::string get(const std::string& key) const;
    int getInt(const std::string& key, int defaultValue = 0) const;
    float getFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool getBool(const std::string& key, bool defaultValue = false) const;
    std::vector<std::string> getList(const std::string& key) const;
    std::vector<int> getIntList(const std::string& key) const;
    std::vector<float> getFloatList(const std::string& key) const;
    std::vector<bool> getBoolList(const std::string& key) const;

    bool set(const std::string& key, const std::string& value);
    bool setInt(const std::string& key, int value);
    bool setFloat(const std::string& key, float value);
    bool setBool(const std::string& key, bool value);

private:
    struct KeyValue {
        std::string key;
        std::string value;
        bool isList = false;
    };

    struct Section {
        std::string name;
        std::vector<KeyValue> items;
    };

    std::vector<KeyValue> globals;
    std::vector<Section> sections;

    const KeyValue* findItem(const std::string& fullKey) const;
    KeyValue* findOrCreateItem(const std::string& fullKey);
    static bool isInlineList(const std::string& val);
    static std::vector<std::string> splitList(const std::string& value);
};

#endif // __YAMLISH_H
