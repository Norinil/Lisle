#include <lisle/create>
#include <string>
#include <iostream>

using namespace std;
using namespace lisle;

void thread0 ()
{
	Thrid self;
	cout << "Hi from thread " << (uint32_t)self << endl;
	cout << "Bye from thread " << (uint32_t)self << endl;
}

void thread1 (Handle<string> arg)
{
	Thrid self;
	string str(*arg);
	cout << "Hi from thread " << (uint32_t)self << endl;
	cout << (uint32_t)self << ": Got argument '";
	cout << str << "'" << endl;
	cout << "Bye from thread " << (uint32_t)self << endl;
}

Handle<string> thread2 ()
{
	Thrid self;
	string ret = "returned string";
	cout << "Hi from thread " << (uint32_t)self << endl;
	cout << "Bye from thread "<< (uint32_t)self;
	cout << ", returning '" << ret << "'" << endl;
	return Handle<string>(new string(ret));
}

Handle<uint32_t> thread3 (Handle<string> arg)
{
	Thrid self;
	uint32_t ret = 0;
	string str(*arg);
	cout << "Hi from thread " << (uint32_t)self << endl;
	cout << (uint32_t)self;
	cout << ": Got argument '" << str << "'" << endl;
	for (size_t i=0; i<str.length(); i++)
		ret += str[i];
	cout << "Bye from thread " << (uint32_t)self;
	cout << ", returning '" << ret << "'" << endl;
	return Handle<uint32_t>(new uint32_t(ret));
}

int main ()
{
	Thrid thread[4];
	Handle<string> argstr(new string("argument string"));
	Handle<string> retstr;
	Handle<uint32_t> retnum;
	cout << "Hi from main thread" << endl;
	thread[0] = create(Thread(&thread0));
	thread[1] = create(Thread(&thread1, argstr));
	thread[2] = create(Thread(&thread2));
	thread[3] = create(Thread(&thread3, argstr));
	thread[0].join();
	thread[1].join();
	thread[2].join(retstr);
	cout << "Got '" << *retstr;
	cout << "' from thread " << (uint32_t)thread[2] << endl;
	thread[3].join(retnum);
	cout << "Got '" << *retnum;
	cout << "' from thread " << (uint32_t)thread[3] << endl;
	cout << "Bye from main thread" << endl;
	return 0;
}
