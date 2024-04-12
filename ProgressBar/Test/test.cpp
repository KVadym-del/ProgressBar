#include "../Headers/ProgressBar.h"

int main(void)
{
	calc_cmd_size();
	ProgressBar statusBar{ {} };
	double var = 0;
	std::wcout << "test\n";

	statusBar.generate_auto(var, 200,
		[&]() {
			for (size_t i = 1; i <= 200; i++)
			{
				var = i;
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
		});
	std::cout << "Done!\n";


	return 0;
}