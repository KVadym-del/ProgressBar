# ProgressBar

A ***ProgressBar*** is a header only C++ library that provides a simple way to display a progress of the event or process through ***progress bar***.

## Usage

Automatic way:
```cpp 
#include "Headers/ProgressBar.h"

...

calc_cmd_size();

ProgressBar statusBar{ {} };
	double var = 0;

	statusBar.generate_auto(var, 200,
		[&]() {
			for (size_t i = 1; i <= 200; i++)
			{
				var = i;
				std::this_thread::sleep_for(std::chrono::milliseconds(500)); // process emulation
			}
		});
...
```

Function `calc_cmd_size()` is used to calculate the size of the console window. It is necessary to correctly display the progress bar.

Class `ProgressBar` is a main class that provides the functionality of the progress bar.

The `generate_auto()` method is used to generate the progress bar. The first parameter is the variable that will be changed during the process. The second parameter is the maximum value of the variable. The third parameter is a lambda function that contains the process that will be displayed in the progress bar.

Manual way:
```cpp 
	auto thread = statusBar.generate_man(var, 200);

	for (size_t i = 1; i <= 200; i++)
	{
		var = i;
		std::this_thread::sleep_for(std::chrono::milliseconds(500)); // process emulation
	}

	thread.join();
```

The `generate_man` method is used to generate the progress bar. The first parameter is the variable that will be changed during the process. The second parameter is the maximum value of the variable.

The method returns a thread that should be joined at the end of the process.

## Details

- To make the progress bar work correctly and not interrupt main thread, ***Progress Bar*** library generates a separate thread that is responsible for the progress bar. The main thread is responsible for the process that will be displayed in the progress bar.

- For dynamic bar sizing and protection from potential leakage library use smart pointers.