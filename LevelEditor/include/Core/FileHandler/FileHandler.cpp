#include "precomp.h"

#include "Core/FileHandler/FileHandler.h"

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



std::string FileHandler::OpenFileWindow(const DialogueBoxArgs& _dialogueBoxArgs)
{
    HRESULT init = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    IFileDialog* pfd = nullptr;

    const CLSID operation = _dialogueBoxArgs.Operation == e_FileOperationType::FILE_LOAD ? CLSID_FileOpenDialog : CLSID_FileSaveDialog;
    HRESULT hr = CoCreateInstance(operation, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
    if (SUCCEEDED(hr))
    {
        FILEOPENDIALOGOPTIONS flags = FOS_FORCEFILESYSTEM;
        if (_dialogueBoxArgs.Operation == e_FileOperationType::FILE_SAVE)
        {
            flags |= FOS_OVERWRITEPROMPT;
        };

        // Customize our dialogue box.
        pfd->SetOptions(flags);
        pfd->SetTitle(_dialogueBoxArgs.WindowTitle.c_str());
        pfd->SetOkButtonLabel(_dialogueBoxArgs.SelectButtonLabel.c_str());
        pfd->SetFileName(_dialogueBoxArgs.InitialFileName.c_str());
        pfd->SetFileTypes(_dialogueBoxArgs.FilterArraySize, _dialogueBoxArgs.FilterTypeArray);

        // Set initial directory and file name
        std::wstring initialDir(_dialogueBoxArgs.InitDir.begin(), _dialogueBoxArgs.InitDir.end());
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
                    std::wstring filePathWStr(pszPath);
                    std::string filePath(filePathWStr.begin(), filePathWStr.end()); // Convert to std::string
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
    return ""; // Return empty string if no file was succesfully selected.
}

