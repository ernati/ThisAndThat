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