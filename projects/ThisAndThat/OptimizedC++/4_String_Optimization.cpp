#include <chrono>
#include <string>
#include <iostream>

using namespace std;

//1. remove_ctrl : 입력으로 받은 문자열에서, 개행문자를 제거하는 기능의 함수

//1.1 remove_ctrl() : 최적화할 곳이 많은 함수
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

// string에서 "+"는 연결 연산자로, 해당 연산자는 다음과 같이 작동한다.
// 1. 메모리 관리자를 호출해 연결된 문자열을 저장하는 임시 문자열 객체를 생성한다.
// 2. 연산 결과를 해당 객체에 담는다.
// 즉, 현재 코드 구조라면, 입력으로 들어온 s의 문자열 길이만큼 메모리 관리자가 호출되어, 시간이 오래 걸리게 된다.



//1.2 (1.1)에서 할당 및 복사 연산을 제거한다.
std::string remove_ctrl_mutating(std::string s)
{
	std::string result;
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] >= 0x20) {
			result += s[i]; // "+=" 연산자로 변경
		}
	}
	return result;
}

// "+="연산자는 임시 문자열 객체를 생성하지 않고, 기존 문자열의 내용을 변경합니다.
// 그러므로, 메모리 관리자가 호출되지 않아서 성능이 늘어납니다.



//1.3 result의 메모리 공간을 미리 확보한다.
std::string remove_ctrl_reserve(std::string s)
{
	std::string result;
	result.reserve(s.length()); // result의 가능한 경우 중 최대 크기인 s의 크기로 미리 공간을 넓혀놓기
	for (int i = 0; i < s.length(); ++i) {
		if (s[i] >= 0x20) {
			result += s[i];
		}
	}
	return result;
}

// 문자열은 미리 확보해놓은 메모리 공간이 다 차면, 현재 크기의 2배로 재할당한다.
// 이 때도 역시 메모리 관리자가 호출되기 때문에, 성능에 영향을 준다.
// 그러므로, 최대 크기를 알 수 있다면, 미리 호출하는 것이 성능에 도움이 된다.



//1.4 const 를 통한 함수 파라미터 복사 제거
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

// 다만, 해당 방법은 오히려 실행이 느려졌는데,
// 그 이유는, 파라미터인 s가 포인터이기 때문이다.
// 참조 변수는 포인터로 구현된다.
// 기존 코드에서는 s를 역참조 할 필요가 없지만(?), 수정된 코드에서는 s가 참조라 포이넡를 역참조 해야한다(?)




//1.5 반복자를 통한 포인터 역참조 제거
std::string remove_ctrl_ref_args_it(std::string const& s)
{
	std::string result;
	result.reserve(s.length()); // result의 가능한 경우 중 최대 크기인 s의 크기로 미리 공간을 넓혀놓기
	for (auto it = s.begin(), end = s.end(); it != end; ++it) {
		if (*it >= 0x20) {
			result += *it;
		}
	}
	return result;
}

// string도 반복자 사용이 가능하구나...?



//1.6 리턴값을 제거하기
void remove_ctrl_ref_result_it(std::string& result, std::string const& s)
{
	result.clear();
	result.reserve(s.length()); // result의 가능한 경우 중 최대 크기인 s의 크기로 미리 공간을 넓혀놓기
	for (auto it = s.begin(), end = s.end(); it != end; ++it) {
		if (*it >= 0x20) {
			result += *it;
		}
	}
}

// RVO : 어떤 함수가 클래스 A의 객체를 생성 후 리턴하고, 그 객체를 변수에 담을 때, 생성자는 총 3번 진행된다.
// 1. 클래스 A의 객체를 생성할 때, // 2. 그 객체를 함수에서 리턴할 때, // 3. 리턴된 객체를 변수에 담을 때
// 이 작업을 한 번의 생성자만 호출시키게 하는 것이 RVO다 ( 컴파일러가 알아서 함수 내부에서 생성된 클래스 A의 객체 주소를 직접 변수에 담는다. )

// 이 과정을 보면, 함수에서 어떤 값을 리턴할 때는 복사(생성자)가 호출되는데, 이를 없애면 성능이 향상한다.



//1.7 std::string 대신 C Style 문자 사용하기
void remove_ctrl_cstrings(char* destp, char const* srcp, size_t size)
{
	for (size_t i = 0; i < size; ++i) {
		if (srcp[i] >= 0x20) *destp++ = srcp[i];
	}
	*destp = 0;
}

// C Style의 문자를 사용하면, 성능이 매우매우 좋아진다!




//2. 더 좋은 알고리즘 사용하기

//2.1 현재 알고리즘은, 한번에 하나의 문자를 확인한 후, 해당 문자를 결과 문자열로 복사한다.
// 이를, "\n"문자를 찾을 때까지 진행한 후, "\n"을 찾으면 지금까지 부분 문자열을 복사한 후, 다음으로 진행하게 변경한다.
std::string remove_ctrl_block(std::string s)
{
	std::string result;
	for (size_t b = 0, i = b, e = s.length(); b < e; b = i + 1) { //3. for문의 3번째 식에 따라, 루프를 돌때마다 b의 값이 i+1로 set 된다. ( 0x20의 다음 위치 )
		//1. b부터 0x20 을 찾는다. 찾았다면, i는 0x20의 위치다.
		for (i = b; i < e; ++i) {
			if (s[i] < 0x20) break;
		}

		//2. 찾았다면, b로부터 i-b 만큼의 부분 문자열을 결과 문자열에 복사한다.
		result = result + s.substr(b, i - b);
	}

	return result;
}

// 이렇게 변경하면, 복사 연산을 덜 할 수 있다.



//2.2 임시 문자열 대신, append 하기
std::string remove_ctrl_block_append(std::string s)
{
	std::string result;
	result.reserve(s.length()); // reserve() 추가
	for (size_t b = 0, i = b, e = s.length(); b < e; b = i + 1) { //3. for문의 3번째 식에 따라, 루프를 돌때마다 b의 값이 i+1로 set 된다. ( 0x20의 다음 위치 )
		//1. b부터 0x20 을 찾는다. 찾았다면, i는 0x20의 위치다.
		for (i = b; i < e; ++i) {
			if (s[i] < 0x20) break;
		}

		//2. 찾았다면, b로부터 i-b 만큼의 부분 문자열을 결과 문자열에 복사한다.
		result.append(s, b, i - b); // append 추가
	}

	return result;
}



//2.3 결과 문자열을 만드는 대신, 원본을 변경
std::string remove_ctrl_erase(std::string s)
{
	for (size_t i = 0; i < s.length();) {
		if (s[i] < 0x20) s.erase(i, 1);
		else ++i;
	}

	return s;
}

//=========================================
// C Style 문자열에서 std::string 으로의 변환
std::string Name() {
	return "MyClass";
}
// 해당 함수는, 리턴 시 문자를 임시 string 객체에 복사합니다.

char const* Name2() {
	return "MyClass";
}
// 해당 함수는, 당연하게도 복사 로직이 없다.


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