#pragma once
#include <filesystem>
#include <span>
#include <cassert>
#include <windows.h>

//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
	//Get the error message ID, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0) {
		return std::string(); //No error message has been recorded
	}

	LPSTR messageBuffer = nullptr;

	//Ask Win32 to give us the string version of that message ID.
	//The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	//Copy the error message into a std::string.
	std::string message(messageBuffer, size);

	//Free the Win32's string's buffer.
	LocalFree(messageBuffer);

	return message;
}

class MmapFile {
public:
  MmapFile(const std::filesystem::path& file_path) {
		if (!std::filesystem::exists(file_path)) {
			throw std::runtime_error("File doesn't exist");
		}
    size_t size = std::filesystem::file_size(file_path);

		HANDLE hMap;
		char* data_ptr = NULL;
		HANDLE hFile = CreateFileA(file_path.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			std::cout << __LINE__ << ": " << GetLastErrorAsString << std::endl;
			return;
		}
		//auto size = GetFileSize(hFile, NULL);
		if (size == INVALID_FILE_SIZE || size == 0) {
			std::cout << __LINE__ << ": " << GetLastErrorAsString << std::endl;
			return;
		}

		hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, size, NULL);
		if (!hMap)
		{
			std::cout << __LINE__ << ": " << GetLastErrorAsString << std::endl;
			return;
		}

		data_ptr = (char*)MapViewOfFile(hMap, FILE_MAP_READ | FILE_MAP_COPY, 0, 0, size);

		// Closing handle will not affect viewed data 
		CloseHandle(hMap);

		data = { data_ptr, size };
  }

	~MmapFile() {
		UnmapViewOfFile(data.data());
	}
	std::span<char> get_file_data() {
		return data;
	}

private:
  std::span<char> data;
};