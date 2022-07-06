#include "Logger.h"
#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>




void Logger::logLevel(Logger::Level level) {
	//https://docs.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
	//the ESC is 0x1B
	/*switch (level) {
	case Logger::Level::Error:
		std::cout << "\x1B[31m[Error] \x1B[91m";
		break;
	case Logger::Level::Warn:
		std::cout << "\x1B[33m[Error] \x1B[93m";
		break;
	default:
		std::cout << "\x1B[36m[Info] \x1B[94m";
		break;
	}*/
	switch (level) {
	case Logger::Level::Error:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		std::cout << "[Error] ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
		break;
	case Logger::Level::Warn:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN);
		std::cout << "[Warn] ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		break;
	default:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE);
		std::cout << "[Info] ";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	}
}

std::ostream& Logger::endlogl(std::ostream& out) {
	out << std::endl;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	return out;
}

extern const char* LAUNCHER_VERSION;
void Logger::Init() {

	if (!consoleAllocated) {
		AllocConsole();
		SetConsoleTitleA("Onix Launcher");
		FILE* f;
		freopen_s(&f, "CONOUT$", "w", stdout);
	}
	consoleAllocated = true;
#ifdef NDEBUG
	HideConsole();
#endif 
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << "================================";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_BLUE);
	std::cout << " Onix Launcher " << LAUNCHER_VERSION;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	std::cout << " ================================" << endlogl;

}

void Logger::deinit() {
	if (consoleAllocated)
		FreeConsole();
	consoleAllocated = false;
}

void Logger::ShowConsole() {
	ShowWindow(GetConsoleWindow(), SW_SHOW);
	//DWORD PrevStyle = SetWindowLongPtrW(GetConsoleWindow(), GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_SYSMENU);
}

void Logger::HideConsole() {
	ShowWindow(GetConsoleWindow(), SW_HIDE);
}

void Logger::log(const std::string& message, Logger::Level level) {
	logLevel(level);
	std::cout << message << endlogl;
}

void Logger::log(const std::string& message, const std::string& message2, Logger::Level level) {
	logLevel(level);
	std::cout << message << message2 << endlogl;
}

void Logger::log(const std::wstring& message, Logger::Level level) {
	logLevel(level);
	std::wcout << message << endlogl;
}

void Logger::log(const std::wstring& message, const std::wstring& message2, Logger::Level level) {
	logLevel(level);
	std::wcout << message << message2 << endlogl;
}

void Logger::log(const char* message, Logger::Level level) {
	logLevel(level);
	std::cout << message << endlogl;
}

void Logger::log(const char* message, const char* message2, Logger::Level level) {
	logLevel(level);
	std::cout << message << message2 << endlogl;
}

void Logger::log(const wchar_t* message, Logger::Level level) {
	logLevel(level);
	std::wcout << message << endlogl;
}

void Logger::log(const wchar_t* message, const wchar_t* message2, Logger::Level level) {
	logLevel(level);
	std::wcout << message << message2 << endlogl;
}

void Logger::logf(const char* format, Logger::Level level, ...) {
	logLevel(level);
	char buff[512];
	va_list args;
	va_start(args, level);
	vsprintf_s(buff, sizeof(buff), format, args);
	std::cout << buff << endlogl;
}

