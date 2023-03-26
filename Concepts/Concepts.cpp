#include <thread>
#include <atomic>
#include <iostream>
#include <Windows.h>
#include <mutex>
#include <stack>

//Общие примеры использования

struct A {
	void operator++(int n) {}
	~A() throw(const char*) {
		if (false)
			throw "test";
	};
};

template <class T>
concept Test = std::is_same_v<T, A> && requires (T a) { a++; }; //requires (T t) {t++;};

template <class A, class B>
concept IS_SAME = std::is_same_v<A, B>;

template <class T>
void foo(T a) requires requires { ++a; a++; a = 0; {a + a} -> IS_SAME<T>; } {
	std::cout << "out1\n";
}

template <class T>
void foo(T a) requires Test<T> {
	std::cout << "out2\n";
}

template <class T>
void foo(T a) {
	std::cout << "out3\n";
}

template <Test T>
void EXAMPLE_1(T) {}

/////////////////////////////

// РАБОТА С auto и концептами
template<typename T>
concept Integral = std::is_integral_v<T>;

template <typename T, size_t Size>
concept NotMoreThan = sizeof(T) <= Size;

Integral auto foo2(Integral auto x) { return x + 10; }

//NotMoreThan<4> auto x1 = foo2(3'000'000'000); //error
  NotMoreThan<4> auto x2 = foo2(2'000'000'000); // ok
////////////////////////////

///// Регулярные выражения ////

void foo3(auto x) {
	if constexpr (requires { *x = 5; }) 
		std::cout << *x;
	else 
		std::cout << x;
}

///////////////////////////////

//// Магия деструктора ////

template <class T>
struct OptionalLike {
	~OptionalLike() requires (std::is_nothrow_destructible_v<T>) = default;
	~OptionalLike() { std::cout << "throw distr\n";/* */ }
};

//////////////////////////


// Сделать requers для конуструкторов методов и диструкторов

// Концепты помогают точно разграничивать неопределенность шаблонов и групировать их

//Folding expressions concepts
template <class ... T> concept Integral2 = (std::is_integral_v<T> && ...);
// Новые лямбда выражения
auto l = []<typename T> requires true
		 (T const& arg) constexpr noexcept
		 -> Integral auto requires (sizeof(T) > 0) 
		 {return 0; };

int main() {
	foo(5); //out1
	foo(5.1); //out1
	foo(A()); //out2
	foo(std::string("test")); //out3

	EXAMPLE_1(A());
	int a = 5;
	foo3(&a);

	OptionalLike<A> like1;
	OptionalLike<int> like2;
}