
//Библиотека из книг на основе std::tuple с variadic templates (автор, название, год и т.д.)

//Для вывода на печать в отсортированном виде по автору, названию и т.д. используются vector или map (быстрее)

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <tuple>
#include <set>
#include <map>
#include <algorithm>

#include <chrono>


template <typename... Args>
class Book {

	template <typename... Args>
	friend class Library;

private:

	std::tuple< Args...> m_data;

public:

	Book(Args&&... args) : m_data(std::forward<Args>(args)...) {}

	template <int V>
	auto&& get() const {
		return std::get<V>(m_data);
	}

	auto operator<=> (const Book<Args...>& other) const = default;


};


template <typename... Args>
class Library {
private:

	std::multiset<Book<Args...>> books;

public:
	Library() = default;

	Library(Library&&) = delete;
	Library& operator=(Library&& str) = delete;
	Library(const Library& other) = delete;
	Library& operator=(const Library& str) = delete;


	Library(std::initializer_list<Book<Args...>>&& book_to_add) {
		books = std::move(book_to_add);
	}

	void add(std::initializer_list<Book<Args...>>&& book_to_add) {
		books.insert(book_to_add);
	}


	template<typename T, std::size_t... Indexes>
	void printBook( T&& book, std::index_sequence<Indexes...> unused) const {

		((std::cout << std::get<Indexes>(book.m_data) << ' '), ...);

		std::cout << '\n';
	}


	void ByAuthor() const {

		std::cout << "\nBy Author:\n";


		std::vector<Book<Args...>> tmp_lib(books.begin(), books.end());

		/*std::sort(tmp_lib.begin(),tmp_lib.end(),												//здесь не надо
			[](const auto& l, const auto& r) { return l.get<0>() < r.get<0>(); }); */

		for (const auto& book : tmp_lib) {
			printBook(std::forward<decltype(book)>(book), std::make_index_sequence<sizeof...(Args)>());
		}



		__asm nop
	}

	void ByTitle() const {			//через map более быстрее

		std::cout << "\nBy Title:\n";
		auto start = std::chrono::system_clock::now();


		/*	std::vector<Book<Args...>> tmp_lib(books.begin(), books.end());
			auto start = std::chrono::system_clock::now();

			std::sort(tmp_lib.begin(), tmp_lib.end(),
				[](const auto& l, const auto& r) { return l.get<1>() < r.get<1>(); });*/		

		auto d = [](const auto& l, const auto& r) { return l.get<1>() < r.get<1>();   };

		std::multiset <Book<Args...>, decltype(d)> tmp_lib(books.begin(), books.end());		


		auto fin = std::chrono::system_clock::now();
		auto g = fin - start;		//753 VS 1141

		for (const auto& book : tmp_lib) {
			printBook(std::forward<decltype(book)>(book), std::make_index_sequence<sizeof...(Args)>());
		}
	}


	void ByYear() const {

		std::cout << "\nBy Year:\n";

		std::vector<Book<Args...>> tmp_lib(books.begin(), books.end());

		std::sort(tmp_lib.begin(), tmp_lib.end(),
			[](const auto& l, const auto& r) { return l.get<2>() < r.get<2>(); });

		for (const auto& book : tmp_lib) {
			printBook(std::forward<decltype(book)>(book), std::make_index_sequence<sizeof...(Args)>());
		}
	}


	void del(const Book<Args...>& book_to_delete) {
		books.erase(book_to_delete);
	}


};



int main() {


	Book<std::string, std::string, int> my_favorite_book{ "Willims", "Parallel C++", 2014 };
	Book<std::string, std::string, int> my_favorite_book_2{ "Straustrup", "C++", 2000 };

	auto info = my_favorite_book.get<1>();

	bool lite = (my_favorite_book < my_favorite_book_2);

	Library<std::string, std::string, int> O;

	Library<std::string, std::string, int> l = { {"Straustrup", "C++", 2000}, my_favorite_book, my_favorite_book_2 };

	
	l.add({ {"Straustrup", "C++", 2010}, {"Lllll", "AAA", 2007}, {"Golovic", "Advanced", 2008} });

	l.ByAuthor();

	l.ByTitle();

	l.ByYear();

	l.del(my_favorite_book_2);
	l.del({ "Straustrup", "C++", 2010 });

	__asm nop


	return 0;
}