#pragma once

#include <iostream>

class BarBuf : public std::wstringbuf
{
public:
    virtual int sync() {
		std::wcout << str();
		str(L"");
		return 0;

    }
};

