#pragma once

constexpr int number_of_names = 10;

class ListOfNames
{
public:
	ListOfNames() 
	{ // TODO Load in names from JSON file?
	};
	~ListOfNames() = default;

	int getNumberOfNames() { return number_of_names; }
	std::string getFirstNameAt(int index) { return firstNames[index]; }
	std::string getNickNameAt(int index) { return nickNames[index]; }
	std::string getLastNameAt(int index) { return lastNames[index]; }

private:
	std::string firstNames[number_of_names];
	std::string nickNames[number_of_names];
	std::string lastNames[number_of_names];
};
