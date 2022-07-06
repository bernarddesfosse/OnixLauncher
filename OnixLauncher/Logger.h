#pragma once
#include <string>
#include <cstdarg>



class Logger {
public:
	
	enum class Level {
		Info = 1,
		Warn = 2,
		Error = 3
	};


	static void Init();
	static void deinit();

	static void ShowConsole();
	static void HideConsole();

	static void log(const std::string& message, Logger::Level level = Logger::Level::Info);
	static void log(const std::string& message, const std::string& message2, Logger::Level level = Logger::Level::Info);
	static void log(const std::wstring& message, Logger::Level level = Logger::Level::Info);
	static void log(const std::wstring& message, const std::wstring& message2, Logger::Level level = Logger::Level::Info);
	static void log(const char* message, Logger::Level level = Logger::Level::Info);
	static void log(const char* message, const char* message2, Logger::Level level = Logger::Level::Info);
	static void log(const wchar_t* message, Logger::Level level = Logger::Level::Info);
	static void log(const wchar_t* message, const wchar_t* message2, Logger::Level level = Logger::Level::Info);
	static void logf(const char* format, Logger::Level level = Logger::Level::Info, ...);

private:
	static inline bool consoleAllocated = false;
	static void logLevel(Logger::Level level);
	static std::ostream& endlogl(std::ostream& out);
};

