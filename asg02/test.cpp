
#include <ctime>
#include <iostream>

#include "Timer.h"

using namespace std;

void TestTimer()
{

	Timer t;
	int x = 1;
	t.Start();
	while( x++ != 100 ) 
	{	
		cout << "Current time is " << t.Elapsed() << endl;
		sleep(1);
		if(x % 6 == 0) 
		{
			t.Stop();
			cout << "Stopped\n";
		}
			
		if(x % 10 == 0 ) 
		{
			t.Start();
			cout << "Restart\n";
		}
		

	}
	t.Stop();

}

int main( void )
{
	TestTimer();
	return 0;
}

