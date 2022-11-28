#include <functional>
#include <iostream>
#include <vector>
#include <cstdint>

// int add_num(int i)
// {
    // return i + 100;
// }

std::vector<uint8_t>* vec_ptr = new std::vector<uint8_t>(100);

[[clang::annotate("offset")]] uint64_t get_offset(uint64_t val) {
	printf("called\n");
	return (uint64_t) vec_ptr + val;
}

// constexpr uint8_t * offset = &vec_ptr;

int main()
{
	// std::function<int(int)> f_offset = add_num;
	
	uint64_t a = 3;
	// int ret = f_offset(a);
	uint64_t [[clang::annotate_type("offset")]] ret = a;
	
	std::cout << ret << std::endl;
}