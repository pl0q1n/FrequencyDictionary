#pragma once
#include <filesystem>
#include <span>
#include <cassert>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

struct Span {
	Span() : data(nullptr), size(0) {}
	Span(char* d, size_t s) : data(d), size(s) {}
	char* data;
	size_t size;
};

class MmapFile {
public:

#ifdef _WIN32
	MmapFile(const std::filesystem::path& file_path) {
		if (!std::filesystem::exists(file_path)) {
			throw std::runtime_error("File doesn't exist");
		}
		size_t size = std::filesystem::file_size(file_path);

		HANDLE map_handle;
		HANDLE file_handle = CreateFileA(file_path.string().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (file_handle == INVALID_HANDLE_VALUE) {
			throw std::runtime_error("Can't create file handle");
			return;
		}

		map_handle = CreateFileMappingA(file_handle, NULL, PAGE_WRITECOPY, 0, size, NULL);
		if (!map_handle)
		{
			CloseHandle(file_handle);
			throw std::runtime_error("Can't create file mapping");
			return;
		}

		char* data_ptr = reinterpret_cast<char*>(MapViewOfFile(map_handle, FILE_MAP_COPY, 0, 0, size));

		// Closing handle will not affect view data
		CloseHandle(map_handle);
		CloseHandle(file_handle);
		data = { data_ptr, size };
	}

#else
	MmapFile(const std::filesystem::path& file_path) {
		if (!std::filesystem::exists(file_path)) {
			throw std::runtime_error("File doesn't exist");
		}

		int file_desc = open(file_path.string().c_str(), O_RDONLY);
		if (file_desc == 0) {
			throw std::runtime_error("Error occured during opening input file");
		}

		size_t size = std::filesystem::file_size(file_path);
		char* data_ptr = reinterpret_cast<char*>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE, file_desc, 0));
		close(file_desc);
		data = { data_ptr, size };
	}
#endif

#ifdef _WIN32
	~MmapFile() {
		UnmapViewOfFile(data.data);
	}
#else 
	~MmapFile() {
		munmap(data.data, data.size);
	}
#endif

	Span get_file_data() {
		return data;
	}

private:
	Span data;
};