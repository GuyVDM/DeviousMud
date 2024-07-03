#pragma once

class FileHandler 
{
public:
	static std::string BrowseFolder(std::string _savedPath);

	static std::string BrowseFile(std::string _savedPath);

	const static bool ExportLevelToJSON();
};