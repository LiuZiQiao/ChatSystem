#include"Window.hpp"
#include<unistd.h>

int main()
{
	Window w;
	w.DrawHeader();
	w.DrawOutput();
	w.DrawOnline();
	w.DrawInput();

	std::string message;
	// w.Welcomde();
	for(;;)
	{
		// std::cin>>message;
		w.GetStringFromInput(message);
		w.PutStringToOutput(message);
	}
	
	// w.PutStringToWin(w.header,2,i,message);
	
	// sleep(2);
	return 0;
}
