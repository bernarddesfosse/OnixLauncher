#include "Utils.h"
#include <Windows.h>
#include <sddl.h>
#include <aclapi.h>
#include <algorithm>

#include <urlmon.h>
#pragma comment(lib, "Urlmon.lib")
#pragma comment(lib, "Advapi32.lib")

extern int ShowGamingMessageBox(const char* titlebar, const char* message, const char* button1, const char* button2, const char* button3);

namespace Utils {




    
    bool setUWPFileAccessRights(const char* filepath) {
        // good starting point
        // https://docs.microsoft.com/en-us/windows/win32/secauthz/creating-a-security-descriptor-for-a-new-object-in-c--
        PSECURITY_DESCRIPTOR securityDescriptor = nullptr;

        ACL* accessControlNow = nullptr;
        if (GetNamedSecurityInfoA(filepath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &accessControlNow, nullptr, &securityDescriptor) != ERROR_SUCCESS)
            return false;

        PSID accountSid = nullptr;
        ConvertStringSidToSidA("S-1-15-2-1", &accountSid);
        if (!securityDescriptor) {
            LocalFree(securityDescriptor);
            return false;
        }

        EXPLICIT_ACCESSA accessPrivileges = { 0 };
        accessPrivileges.grfAccessPermissions = GENERIC_ALL;
        accessPrivileges.grfAccessMode = SET_ACCESS;
        accessPrivileges.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
        accessPrivileges.Trustee.TrusteeForm = TRUSTEE_IS_SID;
        accessPrivileges.Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
        accessPrivileges.Trustee.ptstrName = (char*)accountSid;

        ACL* accessControl = nullptr;
        if (SetEntriesInAclA(1, &accessPrivileges, accessControlNow, &accessControl) == ERROR_SUCCESS) {
            DWORD videoGamed = SetNamedSecurityInfoA((char*)filepath, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, accessControl, nullptr);
            if (accessControl) LocalFree(accessControl);
            LocalFree(securityDescriptor);
            return videoGamed == 0;
        } else {
            LocalFree(securityDescriptor);
            return false;
        }
       
        return false;
    }


    std::string getMD5Hash(const char* filepath) {
		DWORD dwStatus = 0;
		BOOL bResult = FALSE;
		HCRYPTPROV hProv = 0;
		HCRYPTHASH hHash = 0;
		BYTE rgbFile[1024];
		DWORD cbRead = 0;
		BYTE rgbHash[16];
		DWORD cbHash = 0;
        CHAR rgbDigits[] = "0123456789abcdef";

        HANDLE hFile = CreateFileA(filepath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (!hFile) return "";

		if (!CryptAcquireContextA(&hProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return "";

		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) return "";

        while (bResult = ReadFile(hFile, rgbFile, 1024, &cbRead, NULL)) {
            if (cbRead == 0)
                break;

            if (!CryptHashData(hHash, rgbFile, cbRead, 0)) {
                dwStatus = GetLastError();
                CryptReleaseContext(hProv, 0);
                CryptDestroyHash(hHash);
                CloseHandle(hFile);
                return "";
            }
        }


        cbHash = 16;
        std::string result;
        if (bResult && CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
            char temp_buff[32];
            for (DWORD i = 0; i < cbHash; i++) {
                sprintf_s(temp_buff, 32, "%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
                result += temp_buff;
            }
        }
        else {
            dwStatus = GetLastError();
        }

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        CloseHandle(hFile);

		return result;
	}

    std::string getMD5Hash(MemoryBuffer buff) {
        HCRYPTPROV hProv = 0;
        HCRYPTHASH hHash = 0;
        BYTE rgbHash[16];
        DWORD cbHash = 0;
        CHAR rgbDigits[] = "0123456789abcdef";

        if (!CryptAcquireContextA(&hProv, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) return "";

        if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) return "";

        if (!CryptHashData(hHash, (BYTE*)buff.buffer, buff.size, 0)) {
            DWORD dwStatus = GetLastError();
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            return "";
        }


        cbHash = 16;
        std::string result;
        if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbHash, 0)) {
            char temp_buff[32];
            for (DWORD i = 0; i < cbHash; i++) {
                sprintf_s(temp_buff, 32, "%c%c", rgbDigits[rgbHash[i] >> 4], rgbDigits[rgbHash[i] & 0xf]);
                result += temp_buff;
            }
        }
        else {
            DWORD dwStatus = GetLastError();
            int dsadsa = 69;
        }

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);
        return result;
    }


    std::vector<std::string> stringSplit(std::string text, const std::string& splitter) {
        if (splitter.empty()) return std::vector<std::string>{text};
        std::vector<std::string> result;
        size_t nextPos = text.find(splitter);
        while (nextPos != std::string::npos) {
            result.emplace_back(text.substr(0, nextPos));
            text.erase(text.begin(), text.begin() + nextPos + 1);
            nextPos = text.find(splitter);
        }

        result.emplace_back(text);
        return result;
    }

    std::string getTempFile() {
        char* tempFolderPath = new char[512];
        if (GetTempPathA(512, tempFolderPath) == 0) {
            delete[] tempFolderPath;
            return std::string();
        }
        char* appxFilePath = new char[512];
        if (GetTempFileNameA(tempFolderPath, "ONXLCHR", 0, appxFilePath) == 0) {
            delete[] appxFilePath;
            delete[] tempFolderPath;
            return std::string();
        }
        std::string finalPath = appxFilePath;
        delete[] appxFilePath;
        delete[] tempFolderPath;
        return finalPath;
    }

    ClientInformation getClientDllInfo(const char* filepath) {
        ClientInformation retval{};

        HMODULE onixModule = LoadLibraryA(filepath); 
        if (!onixModule) return retval;

        using GetClientVersion_f = const char* (_stdcall*)();
        GetClientVersion_f clientVersionFunc = (GetClientVersion_f)GetProcAddress(onixModule, "GetClientVersion");

        using GetClientVersion_f = const char* (_stdcall*)();
        GetClientVersion_f minecraftVersionFunc = (GetClientVersion_f)GetProcAddress(onixModule, "GetMinecraftVersion");

        if (!clientVersionFunc || !minecraftVersionFunc) return retval;

        retval.clientVersion = clientVersionFunc();
        retval.supportedMinecraftPackageVersions = std::move(Utils::stringSplit(minecraftVersionFunc(), "\n"));

        FreeLibrary(onixModule);

        //convert everything to numbers for easier sorting and reconvertion to visual mc version later
        struct DeserialisedVersion {
            int major, minor, build;
            DeserialisedVersion(int x, int y, int z) : major(x), minor(y), build(z) {}
        };
        std::vector<DeserialisedVersion> rawVersions;
        for (const std::string& versionStr : retval.supportedMinecraftPackageVersions) {
            std::vector<std::string> splittedString = std::move(Utils::stringSplit(versionStr, "."));
            if (splittedString.size() < 3) continue; //cant work with that sorry
            int build = std::atoi(splittedString[2].c_str()); //avoid 0 / 100
            build = build > 0 ? build / 100 : 0;
            rawVersions.emplace_back(std::atoi(splittedString[0].c_str()), std::atoi(splittedString[1].c_str()), build);
        }
            
        std::sort(rawVersions.begin(), rawVersions.end(), [](const DeserialisedVersion& a, const DeserialisedVersion& b) {
            if (a.major != b.major) return a.major > b.major;
            if (a.minor != b.minor) return a.minor > b.minor;
            return a.build > b.build;
            });
        std::sort(retval.supportedMinecraftPackageVersions.begin(), retval.supportedMinecraftPackageVersions.end(), [](const std::string& a, const std::string& b) {
            std::vector<std::string> aStr = Utils::stringSplit(a, ".");
            std::vector<std::string> bStr = Utils::stringSplit(b, ".");
            for (int i = 0; i < 4; i++) {
                int aa = std::atoi(aStr[i].c_str());
                int bb = std::atoi(bStr[i].c_str());
                if (aa != bb) return aa > bb;
            }
            return true; //they are the same
            });

        //convert it to visual version again
        for (const DeserialisedVersion& rawVersion : rawVersions) {
            retval.supportedMinecraftVersions.emplace_back(std::to_string(rawVersion.major) + '.' + std::to_string(rawVersion.minor) + '.' + std::to_string(rawVersion.build));
        }

        retval.foundRequiredInfo = true;
        return retval;
    }


    MemoryBuffer downloadFileInMemory(const char* url) {
        IStream* stream;

        if (URLOpenBlockingStreamA(NULL, url, &stream, NULL, NULL) != S_OK)
            return {};

        DWORD bytesRead = 0;
        DWORD totalSize = 0;
        char buffer[256];
        std::vector<std::pair<int32_t, void*>> chunks;

        while (stream->Read(&buffer, sizeof(buffer), &bytesRead) == S_OK) {
            void* chunk = malloc(bytesRead + 1);
            if (bytesRead == 0) break;
            totalSize += bytesRead;
            if (!chunk) {
                for (auto& ptr : chunks)
                    free(ptr.second);
                stream->Release();
                return {};
            }
            memcpy(chunk, buffer, bytesRead);
            chunks.emplace_back(bytesRead, chunk);
        }
        stream->Release();

        void* retval = malloc(totalSize + 1);
        char* retvalw = (char*)retval;
        if (!retval) {
            for (auto& ptr : chunks)
                free(ptr.second);
            return {};
        }
        for (auto& chunk : chunks) {
            memcpy(retvalw, chunk.second, chunk.first);
            retvalw += chunk.first;
        }

        for (auto& ptr : chunks)
            free(ptr.second);

        return MemoryBuffer(retval, totalSize);
    }


    int showMessageBox(const char* titlebar, const char* message, const char* button1, const char* button2, const char* button3) {
        return ShowGamingMessageBox(titlebar, message, button1, button2, button3);
    }




}