#pragma once
#include <codecvt>
#include <commdlg.h>
#include <locale>
#include <shlobj.h>
#include <Windows.h>

enum class e_FileOperationType 
{
	FILE_LOAD = 0x00,
	FILE_SAVE = 0x01
};

struct DialogueBoxArgs 
{
	//*Customization dialogue box*//
	e_FileOperationType Operation			  = e_FileOperationType::FILE_LOAD;
	std::string			InitDir               = "";
	std::wstring		WindowTitle           = L"Dialogue Box";
	std::wstring		SelectButtonLabel     = L"Select";
	std::wstring		InitialFileName       = L"";

	//To add file filters you need to make an array and follow this example entry:
	// This is explanation of what files | This is what it will actually filter the files on.
	// { L"TestFiles *.test*",              L"*.test* *.etc*" };
	COMDLG_FILTERSPEC*  FilterTypeArray = nullptr;
	U32                 FilterArraySize = 0;
};

/// <summary>
/// https://learn.microsoft.com/nl-nl/windows/win32/learnwin32/example--the-open-dialog-box
/// </summary>
/// <param name="_savedPath"></param>
/// <returns></returns>
class FileHandler 
{
public:
	static std::string OpenFileWindow(const DialogueBoxArgs& _dialogueBoxArgs);
};