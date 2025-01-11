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



//1.6 ���ϰ��� �����ϱ�
void remove_ctrl_ref_result_it(std::string& result, std::string const& s)
{
	result.clear();
	result.reserve(s.length()); // result�� ������ ��� �� �ִ� ũ���� s�� ũ��� �̸� ������ ��������
	for (auto it = s.begin(), end = s.end(); it != end; ++it) {
		if (*it >= 0x20) {
			result += *it;
		}
	}
}

// RVO : � �Լ��� Ŭ���� A�� ��ü�� ���� �� �����ϰ�, �� ��ü�� ������ ���� ��, �����ڴ� �� 3�� ����ȴ�.
// 1. Ŭ���� A�� ��ü�� ������ ��, // 2. �� ��ü�� �Լ����� ������ ��, // 3. ���ϵ� ��ü�� ������ ���� ��
// �� �۾��� �� ���� �����ڸ� ȣ���Ű�� �ϴ� ���� RVO�� ( �����Ϸ��� �˾Ƽ� �Լ� ���ο��� ������ Ŭ���� A�� ��ü �ּҸ� ���� ������ ��´�. )

// �� ������ ����, �Լ����� � ���� ������ ���� ����(������)�� ȣ��Ǵµ�, �̸� ���ָ� ������ ����Ѵ�.



//1.7 std::string ��� C Style ���� ����ϱ�
void remove_ctrl_cstrings(char* destp, char const* srcp, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		if (srcp[i] >= 0x20) *destp++ = srcp[i];
	}
	*destp = 0;
}

// C Style�� ���ڸ� ����ϸ�, ������ �ſ�ſ� ��������!




//2. �� ���� �˰��� ����ϱ�

//2.1 ���� �˰�����, �ѹ��� �ϳ��� ���ڸ� Ȯ���� ��, �ش� ���ڸ� ��� ���ڿ��� �����Ѵ�.
// �̸�, "\n"���ڸ� ã�� ������ ������ ��, "\n"�� ã���� ���ݱ��� �κ� ���ڿ��� ������ ��, �������� �����ϰ� �����Ѵ�.
std::string remove_ctrl_block(std::string s)
{
	std::string result;
	for (size_t b = 0, i = b, e = s.length(); b < e; b = i + 1) { //3. for���� 3��° �Ŀ� ����, ������ �������� b�� ���� i+1�� set �ȴ�. ( 0x20�� ���� ��ġ )
		//1. b���� 0x20 �� ã�´�. ã�Ҵٸ�, i�� 0x20�� ��ġ��.
		for (i = b; i < e; ++i) {
			if (s[i] < 0x20) break;
		}

		//2. ã�Ҵٸ�, b�κ��� i-b ��ŭ�� �κ� ���ڿ��� ��� ���ڿ��� �����Ѵ�.
		result = result + s.substr(b, i - b);
	}

	return result;
}

// �̷��� �����ϸ�, ���� ������ �� �� �� �ִ�.



//2.2 �ӽ� ���ڿ� ���, append �ϱ�
std::string remove_ctrl_block_append(std::string s)
{
	std::string result;
	result.reserve(s.length()); // reserve() �߰�
	for (size_t b = 0, i = b, e = s.length(); b < e; b = i + 1) { //3. for���� 3��° �Ŀ� ����, ������ �������� b�� ���� i+1�� set �ȴ�. ( 0x20�� ���� ��ġ )
		//1. b���� 0x20 �� ã�´�. ã�Ҵٸ�, i�� 0x20�� ��ġ��.
		for (i = b; i < e; ++i) {
			if (s[i] < 0x20) break;
		}

		//2. ã�Ҵٸ�, b�κ��� i-b ��ŭ�� �κ� ���ڿ��� ��� ���ڿ��� �����Ѵ�.
		result.append(s, b, i - b); // append �߰�
	}

	return result;
}



//2.3 ��� ���ڿ��� ����� ���, ������ ����
std::string remove_ctrl_erase(std::string s)
{
	for (size_t i = 0; i < s.length();) {
		if (s[i] < 0x20) s.erase(i, 1);
		else ++i;
	}

	return s;
}

//=========================================
// C Style ���ڿ����� std::string ������ ��ȯ
std::string Name() {
	return "MyClass";
}
// �ش� �Լ���, ���� �� ���ڸ� �ӽ� string ��ü�� �����մϴ�.

char const* Name2() {
	return "MyClass";
}
// �ش� �Լ���, �翬�ϰԵ� ���� ������ ����.


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

	std::string test6 = "";
	StartTime = chrono::system_clock::now();
	remove_ctrl_ref_result_it(test6, test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	char szDest[256] = { 0, };
	char szTest[256] = "jdaslkjflakj\nkldjaksjhfkajhfkjs\nkdjsakljdfjajks";
	StartTime = chrono::system_clock::now();
	remove_ctrl_cstrings(szDest, szTest, 29);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test7 = remove_ctrl_block(test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test8 = remove_ctrl_block_append(test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	StartTime = chrono::system_clock::now();
	std::string test9 = remove_ctrl_block_append(test);
	EndTime = chrono::system_clock::now();
	nano = EndTime - StartTime;
	cout << "Time : " << nano.count() << endl;

	return 0;
}