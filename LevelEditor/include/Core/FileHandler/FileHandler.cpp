#include "precomp.h"

#include "Core/FileHandler/FileHandler.h"

#include <codecvt>
#include <commdlg.h>
#include <locale>
#include <shlobj.h>
#include <Windows.h>

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

std::string FileHandler::SaveFile(const std::string& _savedPath)
{
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IFileDialog* pfd = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        pfd->SetOptions(FOS_OVERWRITEPROMPT | FOS_FORCEFILESYSTEM);

        // Set initial directory and file name
        std::wstring initialDir(_savedPath.begin(), _savedPath.end());
        IShellItem* psiInitial = nullptr;
        hr = SHCreateItemFromParsingName(initialDir.c_str(), nullptr, IID_PPV_ARGS(&psiInitial));
        if (SUCCEEDED(hr))
        {
            pfd->SetFolder(psiInitial);
            pfd->SetFileName(initialDir.c_str());
            psiInitial->Release();
        }

        // Show the dialog
        hr = pfd->Show(nullptr);
        if (SUCCEEDED(hr))
        {
            // Get the selected file
            IShellItem* psiResult = nullptr;
            hr = pfd->GetResult(&psiResult);
            if (SUCCEEDED(hr))
            {
                // Get the file path
                PWSTR pszPath = nullptr;
                hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &pszPath);
                if (SUCCEEDED(hr))
                {
                    // Convert to std::string
                    std::wstring filePathWStr(pszPath);
                    std::string filePath(filePathWStr.begin(), filePathWStr.end());
                    CoTaskMemFree(pszPath); // Free the path memory
                    psiResult->Release();
                    pfd->Release();
                    CoUninitialize(); // Uninitialize COM library
                    return filePath;
                }
                psiResult->Release();
            }
        }
        pfd->Release();
    }

    CoUninitialize(); // Uninitialize COM library
    return ""; // Return empty string if no file was selected or an error occurred
}

