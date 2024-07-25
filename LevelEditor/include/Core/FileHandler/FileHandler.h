#pragma once

/// <summary>
/// https://learn.microsoft.com/nl-nl/windows/win32/learnwin32/example--the-open-dialog-box
/// </summary>
/// <param name="_savedPath"></param>
/// <returns></returns>
class FileHandler 
{
public:
	static std::string SaveFile(const std::string& _savedPath);
};