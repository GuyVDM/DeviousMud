#include "precomp.h"

#include "Core/FileHandler/FileHandler.h"

#include <codecvt>
#include <commdlg.h>
#include <locale>
#include <shlobj.h>
#include <Windows.h>

const bool FileHandler::ExportLevelToJSON() 
{
    using json = nlohmann::json;

    int test = 42;

    json j;
    j["value"] = test;

    std::string filePath = "saved/dmlevel.json";

    std::ofstream outputFile(filePath);

    if (!outputFile.is_open())
    {
        DEVIOUS_ERR("Failed to open file.");
        return false;
    }
    else
    {
        outputFile << j.dump(4);

        outputFile.close();

        DEVIOUS_EVENT("Succesfully exported [saved/dmlevel.json]");
        return true;
    }

    return false;
}

static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED)
    {
        std::string tmp = (const char*)lpData;
        std::cout << "path: " << tmp << std::endl;
        SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
    }

    return 0;
}

std::string FileHandler::BrowseFolder(std::string saved_path)
{
    TCHAR path[MAX_PATH];

    const char* path_param = saved_path.c_str();

    BROWSEINFO bi = { 0 };
    bi.lpszTitle = L"Browse for folder...";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn = BrowseCallbackProc;
    bi.lParam = (LPARAM)path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

    if (pidl != 0)
    {
        //Get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //Free memory used
        IMalloc* imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc)))
        {
            imalloc->Free(pidl);
            imalloc->Release();
        }

        std::wstring wPath = path;
        return std::string(wPath.begin(), wPath.end());
    }

    return "";
}

std::string FileHandler::BrowseFile(std::string _savedPath)
{
    OPENFILENAME ofn;      
    WCHAR szFile[260];

    std::wstring initialDir = std::wstring(_savedPath.begin(), _savedPath.end());

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"All\0*.*\0Text\0*.TXT\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = initialDir.c_str();
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn) == TRUE)
    {
        std::wstring filePathWStr(ofn.lpstrFile);

        std::string filePath(filePathWStr.begin(), filePathWStr.end());

        return filePath;
    }

    return "";
}
