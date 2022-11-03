#ifndef LN_h
#define LN_h

#include <string_view>

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <string>
#include <utility>
#include <vector>

class LN
{
  private:
	bool sign = 0;
	std::vector< std::uint32_t > data;
	bool NaN = 0;

	explicit LN(bool sign, std::vector< std::uint32_t > &&data);

	static LN sum(const LN &lhs, const LN &rhs);
	static LN sub(const LN &lhs, const LN &rhs);
	static void add_at_pos(LN &num, std::uint32_t u_rank, std::size_t pos);
	static LN mul_uint(const LN &lhs, const std::uint32_t rhs);
	static std::pair< LN, LN > div_QR(const LN &lhs, const LN &rhs);
	static bool modul_less(const LN &lhs, const LN &rhs);
	static bool modul_less_or_equal(const LN &lhs, const LN &rhs);
	static LN get_num(const char *num, std::size_t size);
	static inline bool get_bit(std::size_t ind, std::uint32_t num);
	static inline void set_bit(std::size_t ind, std::uint32_t &num, bool value);

  public:
	void shift_one_bit();
	void shift_one_bit_right();

  private:
	std::size_t get_normalized_ind() const;
	void normalize();

  public:
	LN(long long num = 0LL);
	explicit LN(const char *num);
	explicit LN(const std::string_view &num);
	explicit LN(const std::string &num);

	LN(const LN &other) = default;
	LN(LN &&other) = default;
	LN &operator=(const LN &other) = default;
	LN &operator=(LN &&other) = default;

	friend LN operator+(const LN &lhs, const LN &rhs);
	friend LN &operator+=(LN &lhs, const LN &rhs);

	friend LN operator-(const LN &lhs, const LN &rhs);
	friend LN &operator-=(LN &lhs, const LN &rhs);

	friend LN operator*(const LN &lhs, const LN &rhs);
	friend LN &operator*=(LN &lhs, const LN &rhs);

	friend LN operator/(const LN &lhs, const LN &rhs);
	friend LN &operator/=(LN &lhs, const LN &rhs);

	friend LN operator%(const LN &lhs, const LN &rhs);
	friend LN &operator%=(LN &lhs, const LN &rhs);

	friend LN operator<<(LN &&lhs, const std::size_t bits);
	friend LN operator<<(const LN &lhs, const std::size_t bits);

	LN &operator-();
	LN &operator~();

	operator long long() const;
	operator bool() const;

	friend bool operator<(const LN &lhs, const LN &rhs);
	friend bool operator<=(const LN &lhs, const LN &rhs);
	friend bool operator>(const LN &lhs, const LN &rhs);
	friend bool operator>=(const LN &lhs, const LN &rhs);
	friend bool operator==(const LN &lhs, const LN &rhs);
	friend bool operator!=(const LN &lhs, const LN &rhs);

	friend std::istream &operator>>(std::istream &input, LN &num);
	friend std::ostream &operator<<(std::ostream &out, const LN &num);

	bool isNaN() const;
};

LN operator"" _ln(const char *num);

#endif
