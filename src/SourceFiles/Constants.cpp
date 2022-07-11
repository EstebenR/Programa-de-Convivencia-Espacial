#include "Constants.h"
#include <fstream>
#include <iostream>

Constants::Constants(const std::string &path)
{
	std::ifstream read(path);
#ifdef _DEBUG
	char fullPath[_MAX_PATH];
	_fullpath(fullPath, path.c_str(), _MAX_PATH);
	std::cout << "looking for constants in:" << fullPath << std::endl;
#endif

	if (!read.is_open())
		throw std::runtime_error("No se ha abierto el archivo de constantes"); //hehe
	read >> data;
	initialized_ = true;
}

//La plantilla gen�rica no hace comprobaciones de tipo
template <typename T>
T Constants::getConstant(const std::string &key) const
{
	T d;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null())
		try
		{
			d = data[key].get<T>();
			return d;
		}
		catch (...)
		{
			throw "Incorrect constant";
		}
}
template <>
int Constants::getConstant<int>(const std::string &key) const
{
	int pt = 0;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_number_integer())
		try
		{
			pt = data[key].get<int>();
			return pt;
		}
		catch (...)
		{
			throw "Incorrect string constant";
		}
}
template <>
bool Constants::getConstant<bool>(const std::string &key) const
{
	bool r;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_boolean())

		r = data[key].get<bool>();
	return r;
}
template <>
double Constants::getConstant<double>(const std::string &key) const
{
	double pt = 0;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_number())

		pt = data[key].get<double>();
	return pt;
}
template <>
std::string Constants::getConstant<std::string>(const std::string &key) const
{
	std::string pt = "";
	//std::cout << "BUSCANDO " << key << "\n";

	if (!data[key].is_null() && data[key].is_string())
		pt = data[key].get<std::string>();
	return pt;
}
template <>
float Constants::getConstant<float>(const std::string &key) const
{
	float pt = 0.0;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_number())
		pt = data[key].get<float>();
	return pt;
}