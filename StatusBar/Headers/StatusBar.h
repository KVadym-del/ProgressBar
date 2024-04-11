#pragma once

#include <Windows.h>

#include <cstdint>
#include <iostream>
#include <exception>
#include <memory>
#include <thread>
#include <cmath>

std::uint32_t floorToOdd(double num) {
	std::uint32_t floored = static_cast<std::uint32_t>(std::floor(num));

	if (floored % 2 == 0) {
		floored--;
	}
	return floored;
}


std::int32_t cmd_columns = 0, cmd_rows = 0;

static inline void calc_cmd_size(void) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	cmd_columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
	cmd_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

typedef struct BarBody {
	inline BarBody(
		std::uint32_t height = 3,
		std::uint32_t width = floorToOdd(((static_cast<double>(80) * cmd_columns) / 100)),
		wchar_t cornerTL	= L'/',
		wchar_t cornerTR	= L'\\',
		wchar_t topDown		= L'-',
		wchar_t leftRight	= L'|',
		wchar_t unfinished	= L' ',
		wchar_t finished	= L'X'
	) : 
		height(height), 
		width(width), 
		cornerTL(cornerTL), 
		cornerTR(cornerTR), 
		topDown(topDown), 
		leftRight(leftRight), 
		unfinished(unfinished), 
		finished(finished)
	{
		if (cmd_columns == 0)	throw std::exception("Error: cmd_columns uncalculated. Check if calc_cmd_size() called!");
		if (cmd_rows == 0)		throw std::exception("Error: cmd_rows uncalculated. Check if calc_cmd_size() called!");
	
		for (std::uint32_t i = 0; i < height; ++i) {
			canvas[i] = std::make_unique<wchar_t[]>(width);
		}
	}
	
	std::uint32_t width;
	std::uint32_t height;

	wchar_t cornerTL;
	wchar_t cornerTR;
	wchar_t topDown;
	wchar_t leftRight;
	wchar_t unfinished;
	wchar_t finished;

	std::unique_ptr<std::unique_ptr<wchar_t[]>[]> canvas = std::make_unique<std::unique_ptr<wchar_t[]>[]>(height);
} BarBody;

class StatusBar 
{
public:
	inline StatusBar(BarBody orgBarBody, std::int64_t statusBarDelay = 10)
		: barBody(std::move(orgBarBody)),
		statusBarDelay(statusBarDelay)
	{
		for (std::size_t y = 0; y < barBody.height; y++)
		{
			for (std::size_t x = 0; x < barBody.width; x++) 
			{
				if (y == 0 && x == 0)									{ barBody.canvas[y][x] = barBody.cornerTL; continue; }
				if (y == 0 && x == barBody.width - 1)					{ barBody.canvas[y][x] = barBody.cornerTR; continue; }
				if (y == barBody.height - 1 && x == 0)					{ barBody.canvas[y][x] = barBody.cornerTR; continue; }
				if (y == barBody.height - 1 && x == barBody.width - 1)	{ barBody.canvas[y][x] = barBody.cornerTL; continue; }
				
				if ((y != 0 && y != barBody.height - 1) && (x == 0 || x == barBody.width - 1)) { barBody.canvas[y][x] = barBody.leftRight; continue; }
				if ((x != 0 && x != barBody.width - 1) && (y == 0 || y == barBody.height - 1)) { barBody.canvas[y][x] = barBody.topDown; continue; }

				barBody.canvas[static_cast<std::size_t>((static_cast<double>(y) / 2) + 0.5)][x] = barBody.unfinished;
			}
		}
	}

	template<typename Body>
	inline void generate_auto(const double& index, double max, Body body)
	{
		auto thread = generate_man(index, max);
		body();
		thread.join();
	}

	[[nodiscard]] inline std::thread generate_man(const double& index, double max)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		this->max = max;
		return std::thread([&]() { print_thread(std::cref(index)); });
	}

private:
    inline void print_thread(const double& index) 
    { 
		const std::wostream oldWcout{ std::wcout.rdbuf() };

		float indexPrc = static_cast<float>((index * 100) / max);
		float oldIndexPrc{ 0 };
		do  {
			indexPrc = static_cast<float>((index * 100) / max);
			if (std::floor(oldIndexPrc) == std::floor(indexPrc)) continue;

			std::this_thread::sleep_for(std::chrono::milliseconds(statusBarDelay));
			system("cls");

			int x = static_cast<int>((indexPrc * barBody.width) / 100);
			if (x == barBody.width) x = barBody.width - 2;
			auto barMiddle = static_cast<std::size_t>((static_cast<double>(barBody.height) / 2));
			for (std::size_t startX = 1; startX <= x; startX++)
			{
				barBody.canvas[barMiddle][startX] = barBody.finished;
			}

			for (std::uint32_t i = 0; i < barBody.height; ++i) {
				if (i == barMiddle)
				{
					std::wcout.write(barBody.canvas[i].get(), barBody.width);
					std::wcout << '\t' << std::ceil(indexPrc) << "%"; // TODO: add mor control over the data
				}
				else 
				{
					std::wcout.write(barBody.canvas[i].get(), barBody.width);
				}
				
				std::wcout << "\n";
			}
			oldIndexPrc = indexPrc;
		} while (indexPrc < 100);
    }

private:
	const std::int64_t statusBarDelay;

	BarBody barBody;

	double max{1};

};
