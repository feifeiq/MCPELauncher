#pragma once
#include <string>
class ResourceLocation {
public:
	std::string str1; // 0
	int position; // 4
	std::string something; // 8
	/*ResourceLocation(std::string const& _str1) : ResourceLocation() {
		*((void**)(&str1)) = *((void**)&_str1);
		position = 0;
	}*/
	ResourceLocation(std::string const& _str1, int _position=0, std::string _something="AAAAAAAAAA") : str1(_str1), position(_position), something(_something) {
	}
	ResourceLocation();
	~ResourceLocation();
};

