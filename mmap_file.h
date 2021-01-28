#pragma once
#include <filesystem>
#include <span>
#include <cassert>
#include <windows.h>

constexpr int BUF_SIZE = 256;


class MmapFile {
public:
  MmapFile(const std::filesystem::path& file_path) {
    assert(std::filesystem::exists(file_path));
    size_t size = std::filesystem::file_size(file_path);

		HANDLE hMap;
		char* data_ptr = NULL;
		HANDLE hFile = CreateFileA(file_path.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
			return;

		//auto size = GetFileSize(hFile, NULL);
		if (size == INVALID_FILE_SIZE || size == 0)
			return;

		hMap = CreateFileMappingA(hFile, NULL, PAGE_READONLY, 0, size, NULL);
		if (!hMap)
			return;

		data_ptr = (char*)MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, size);

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