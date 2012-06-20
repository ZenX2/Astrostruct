/**
* @file NConfig.hpp
* @brief Reads information from a lua file and lets other parts of the engine access the data from it.
* @author Dalton Nell
* @version 1.0
* @date 2012-05-03
*/
#ifndef NAELSTROF_CONFIG
#define NAELSTROF_CONFIG

/**
* @brief Provides an interface to read data from a file and expose it to the rest of the engine.
*/
class NConfig
{
public:
	/**
	* @brief Reads the file through lua to read configuration variables.
	*
	* @param i_File The lua file.
	*/
	NConfig(std::string i_Folder);
	~NConfig();
	/**
	* @brief Finds a variable loaded into lua and returns it.
	*
	* @param Name The name of the variable.
	*
	* @return The value of the variable.
	*/
	float GetFloat(std::string Name);
	/**
	* @brief Finds a variable loaded into lua and returns it.
	*
	* @param Name The name of the variable.
	*
	* @return The value of the variable.
	*/
	std::string GetString(std::string Name);
	/**
	* @brief Finds a variable loaded into lua and returns it.
	*
	* @param Name The name of the variable.
	*
	* @return The value of the variable.
	*/
	bool GetBool(std::string Name);
private:
	std::string Folder;
};

#endif
