#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <vector>

using namespace std;
mutex ready, is_giving, writer;
condition_variable on_give, on_ready;
int counter = 0;
int choice;
bool is_notified = false;
bool is_ready;

void writeInConsole(string str){
	unique_lock<mutex> lock(writer);
	cout << str << endl;
}
void actionsOfSmoker(int id) {	
	{
		unique_lock<mutex> lock(ready);		
		if (id == 1) {
			writeInConsole("Smoker with tobacco is ready!");
		}
		else if (id == 2) {
			writeInConsole("Smoker with paper is ready!");
		}
		else if (id == 3) {
			writeInConsole("Smoker with matches is ready!");
		}		
		counter++;
		this_thread::sleep_for(chrono::milliseconds(1000));		
		if (counter == 3)
		{
			is_ready = true;
			on_ready.notify_one();
		}		
	}
	{
		unique_lock<mutex> lock(is_giving);
		while (!is_notified)
			on_give.wait(lock);
		if (id == choice)
		{
			if (id == 1) {
				writeInConsole("Smoker with tobacco smokes. Other smokers are waiting...");
			}
			else if (id == 2) {
				writeInConsole("Smoker with paper smokes. Other smokers are waiting...");
			}
			else if (id == 3) {
				writeInConsole("Smoker with matches smokes. Other smokers are waiting...");
			}
		}
		counter--;
	}
}
void actionsOfAgent() {
	srand(time(nullptr));
	while (true) {
		unique_lock<mutex> lock(ready);		
		while (!is_ready)
			on_ready.wait(lock);
		writeInConsole("Agent is choosing two things...");
		this_thread::sleep_for(chrono::milliseconds(1500));
		choice = (rand() % 3) + 1;
		is_ready = false;
		if (choice == 1) {
			writeInConsole("Agent choose paper and matches!");
		}
		else if (choice == 2) {
			writeInConsole("Agent choose tobacco and matches!");
		}
		else if (choice == 3) {
			writeInConsole("Agent choose tobacco and paper!");
		}		
		is_notified = true;
		on_give.notify_all();
	}
}
void checkForInput(int number) {
	if (number <= 0)
	{		
		throw exception("Incorrect input!");
	}
}
int main()
{
	cout << "Enter number of iterations: ";
	int number, k = 0;
	cin >> number;
	checkForInput(number);	
	thread agent(actionsOfAgent);
	vector<thread> smokers;
	while (k < number)
	{
		smokers.clear();
		smokers.push_back(thread(actionsOfSmoker, 1));
		smokers.push_back(thread(actionsOfSmoker, 2));
		smokers.push_back(thread(actionsOfSmoker, 3));
		for (size_t i = 0; i < 3; i++)
		{
			smokers[i].join();
		}			
		is_notified = false;
		is_ready = false;
		choice = 0;
		k++;
		cout << "_____________________________That_was_" << k << "_iteretion" << endl;
	}

	agent.detach();
}