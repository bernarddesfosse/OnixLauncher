#pragma once
#include <string>
#include <vector>
#include <functional>


struct ClientInformation {
	bool foundRequiredInfo = false;
	std::string clientVersion;
	std::vector <std::string> supportedMinecraftPackageVersions;
	std::vector <std::string> supportedMinecraftVersions;

	operator bool() { return foundRequiredInfo; }
};


struct MemoryBuffer {
	void* buffer = nullptr;
	uint32_t size = 0;
	operator bool() { return buffer != nullptr && size > 0; }

	MemoryBuffer(void* buffer, uint32_t size) {
		this->buffer = buffer;
		this->size = size;
		refcount = new int16_t(1);
	}
	MemoryBuffer() {
		refcount = new int16_t(1);
	}
	MemoryBuffer(const MemoryBuffer& other) {
		refcount = other.refcount; 
		(*refcount)++;
		this->buffer = other.buffer;
		this->size = other.size;
	}
	MemoryBuffer(MemoryBuffer&& other) noexcept {
		memcpy(&other, this, sizeof(*this));
		memset(&other, 0, sizeof(other));
	}
	~MemoryBuffer() {
		(*refcount)--;
		if (*refcount <= 0) {
			delete refcount;
			free(buffer);
		}
	}

	void writeToFile(const char* filepath) {
		FILE* f;
		fopen_s(&f, filepath, "wb");
		fwrite(this->buffer, this->size, 1, f);
		fclose(f);
	}
private:
	int16_t* refcount = 0;
};


template <typename... Args>
class Event {
	using FunctionType = void(Args...);
	std::vector<std::function<FunctionType>> mCallbacks;
public:
	void operator +=(std::function<FunctionType> rhs) {
		mCallbacks.emplace_back(rhs);
	}


	void operator()(Args... args) {
		for (auto& callback : mCallbacks)
			callback(args...);
	}

	auto begin() {
		return mCallbacks.begin();
	}
	auto end() {
		return mCallbacks.end();
	}
};


namespace Utils {
	bool setUWPFileAccessRights(const char* filepath);
	std::string getMD5Hash(const char* filepath);
	std::string getMD5Hash(MemoryBuffer buff);
	std::vector<std::string> stringSplit(std::string text, const std::string& splitter);

	std::string getTempFile();

	ClientInformation getClientDllInfo(const char* filepath);

	MemoryBuffer downloadFileInMemory(const char* url);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="button1"></param>
	/// <param name="button2"></param>
	/// <param name="button3"></param>
	/// <returns>0 is closed/none/cancel, 1 is button 1, 2 is button 2, 3 is button 3</returns>
	int showMessageBox(const char* titlebar, const char* message, const char* button1 = "Ok", const char* button2 = nullptr, const char* button3 = nullptr);
}