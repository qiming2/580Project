#pragma once
#include "iostream"
namespace KT {
	// simple variadic print function for debugging purposes
	// this is not recommended in practice! would
	// drastically increase code size
	template<typename T>
	void println(T&& t) {
		std::cout << t << "\n";
	}

	template<typename T, typename... Args>
	void println(T&& t, Args&&... args) {
		std::cout << t << "\n";
		println(args...);
	}

	template<typename T>
	void printOneLine(T&& t) {
		std::cout << " " << t;
	}

	template<typename T, typename... Args>
	void printOneLine(T&& t, Args&&... args) {
		std::cout << " " << t;©
		printOneLine(args...);
	}

	template<typename T>
	void print(T&& t) {
		std::cout << t << "\n";
	}

	template<typename T, typename... Args>
	void print(T&& t, Args&&... args) {
		std::cout << t;
		printOneLine(args...);
		std::cout << "\n";
	}
}
