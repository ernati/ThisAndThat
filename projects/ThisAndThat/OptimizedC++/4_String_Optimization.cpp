#include <chrono>
#include <string>
#include <iostream>

using namespace std;

//1. remove_ctrl : �Է����� ���� ���ڿ�����, ���๮�ڸ� �����ϴ� ����� �Լ�

//1.1 remove_ctrl() : ����ȭ�� ���� ���� �Լ�
std::string remove_ctrl(std::string s)
{
	std::string result;
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] >= 0x20) {
			result = result + s[i];
		}
	}
	return result;
}

// string���� "+"�� ���� �����ڷ�, �ش� �����ڴ� ������ ���� �۵��Ѵ�.
// 1. �޸� �����ڸ� ȣ���� ����� ���ڿ��� �����ϴ� �ӽ� ���ڿ� ��ü�� �����Ѵ�.
// 2. ���� ����� �ش� ��ü�� ��´�.
// ��, ���� �ڵ� �������, �Է����� ���� s�� ���ڿ� ���̸�ŭ �޸� �����ڰ� ȣ��Ǿ�, �ð��� ���� �ɸ��� �ȴ�.



//1.2 (1.1)���� �Ҵ� �� ���� ������ �����Ѵ�.
std::string remove_ctrl_mutating(std::string s)
{
	std::string result;
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] >= 0x20) {
			result += s[i]; // "+=" �����ڷ� ����
		}
	}
	return result;
}

// "+="�����ڴ� �ӽ� ���ڿ� ��ü�� �������� �ʰ�, ���� ���ڿ��� ������ �����մϴ�.
// �׷��Ƿ�, �޸� �����ڰ� ȣ����� �ʾƼ� ������ �þ�ϴ�.



//1.3 result�� �޸� ������ �̸� Ȯ���Ѵ�.
std::string remove_ctrl_reserve(std::string s)
{
	std::string result;
	result.reserve(s.length()); // result�� ������ ��� �� �ִ� ũ���� s�� ũ��� �̸� ������ ��������
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] >= 0x20) {
			result += s[i];
		}
	}
	return result;
}

// ���ڿ��� �̸� Ȯ���س��� �޸� ������ �� ����, ���� ũ���� 2��� ���Ҵ��Ѵ�.
// �� ���� ���� �޸� �����ڰ� ȣ��Ǳ� ������, ���ɿ� ������ �ش�.
// �׷��Ƿ�, �ִ� ũ�⸦ �� �� �ִٸ�, �̸� ȣ���ϴ� ���� ���ɿ� ������ �ȴ�.



//1.4 const �� ���� �Լ� �Ķ���� ���� ����
std::string remove_ctrl_ref_args(std::string const& s) // const
{
	std::string result;
	result.reserve(s.length()); 
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] >= 0x20) {
			result += s[i];
		}
	}
	return result;
}

// �ٸ�, �ش� ����� ������ ������ �������µ�,
// �� ������, �Ķ������ s�� �������̱� �����̴�.
// ���� ������ �����ͷ� �����ȴ�.
// ���� �ڵ忡���� s�� ������ �� �ʿ䰡 ������(?), ������ �ڵ忡���� s�� ������ ���̆��� ������ �ؾ��Ѵ�(?)




//1.5 �ݺ��ڸ� ���� ������ ������ ����
std::string remove_ctrl_ref_args_it(std::string const& s)
{
	std::string result;
	result.reserve(s.length()); // result�� ������ ��� �� �ִ� ũ���� s�� ũ��� �̸� ������ ��������
	for (auto it = s.begin(), end = s.end(); it != end; ++it) {
		if (*it >= 0x20) {
			result += *it;
		}
	}
	return result;
}

// string�� �ݺ��� ����� �����ϱ���...?


int main(void)
{
	chrono::system_clock::time_point StartTime = chrono::system_clock::now();
	chrono::system_clock::time_point EndTime = chrono::system_clock::now();

	std::string test = "jdaslkjflakj\nkldjaksjhfkajhfkjs\nkdjsakljdfjajks";

	StartTime = chrono::system_clock::now();
	std::string test1 = remove_ctrl(test);
	EndTime = chrono::system_clock::now();
	chrono::nanoseconds nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test2 = remove_ctrl_mutating(test);
	EndTime = chrono::system_clock::now();
    nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test3 = remove_ctrl_reserve(test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test4 = remove_ctrl_ref_args(test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test5 = remove_ctrl_ref_args_it(test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	return 0;
}