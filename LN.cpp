#include "LN.h"

LN::LN(long long num) : data(2)
{
	if (num < 0)
	{
		sign = 1;
		num = -num;
	}
	data[0] = static_cast< std::uint32_t >(num);
	data[1] = num >> 32;
	this->normalize();
}

LN LN::get_num(const char *num, std::size_t size)
{
	if (size == 3 && num[0] == 'N')
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	bool sign = 0;
	if (num[0] == '-')
		sign = 1;
	long long tmp_num = 0;
	std::size_t last_ind = sign;
	for (std::size_t i = sign; i - sign < (size - sign) % 9; i++)
	{
		tmp_num = tmp_num * 10 + (num[i] - '0');
		last_ind = i + 1;
	}
	LN res(tmp_num);
	tmp_num = 0;
	for (std::size_t i = last_ind; i < size; i++)
	{
		tmp_num = tmp_num * 10 + (num[i] - '0');
		if ((i - last_ind) % 9 == 8)
		{
			res = mul_uint(res, 1'000'000'000) + LN(tmp_num);
			tmp_num = 0;
		}
	}
	res.sign = sign;
	res.normalize();
	return res;
}

LN::LN(const char *num)
{
	*this = LN::get_num(num, std::strlen(num));
}

LN::LN(const std::string_view &num)
{
	*this = LN::get_num(num.data(), num.size());
}

LN::LN(const std::string &num)
{
	*this = LN::get_num(num.data(), num.size());
}

std::size_t LN::get_normalized_ind() const
{
	auto it_of_ind = std::find_if(data.rbegin(), data.rend(), [](std::uint32_t num) { return num != 0; });
	if (it_of_ind == data.rend())
		return 0;
	return data.rend() - it_of_ind - 1;
}

LN::LN(bool sign, std::vector< std::uint32_t > &&data) : sign(sign), data(std::move(data))
{
	normalize();
}

void LN::normalize()
{
	if (isNaN())
	{
		data.clear();
		return;
	}
	std::size_t ind = get_normalized_ind();
	data.resize(ind + 1);
	if (ind == 0 && data[0] == 0)
		sign = 0;
}

bool LN::modul_less(const LN &lhs, const LN &rhs)
{
	if (lhs.data.size() != rhs.data.size())
		return lhs.data.size() < rhs.data.size();
	for (std::size_t i = lhs.data.size() - 1; i > 0; i--)
	{
		if (rhs.data[i] != lhs.data[i])
			return lhs.data[i] < rhs.data[i];
	}
	return lhs.data[0] < rhs.data[0];
}

bool LN::modul_less_or_equal(const LN &lhs, const LN &rhs)
{
	if (lhs.data.size() != rhs.data.size())
		return lhs.data.size() < rhs.data.size();
	for (std::size_t i = lhs.data.size() - 1; i > 0; i--)
	{
		if (rhs.data[i] != lhs.data[i])
			return lhs.data[i] <= rhs.data[i];
	}
	return lhs.data[0] <= rhs.data[0];
}

bool operator<(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
		return false;
	if (lhs.sign != rhs.sign)
		return lhs.sign;
	if (lhs == rhs)
		return false;
	return LN::modul_less(lhs, rhs) ^ lhs.sign;
}

bool operator<=(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
		return false;
	if (lhs.sign != rhs.sign)
		return lhs.sign;
	if (lhs == rhs)
		return true;
	return LN::modul_less_or_equal(lhs, rhs) ^ lhs.sign;
}

bool operator>(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
		return false;
	return rhs < lhs;
}

bool operator>=(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
		return false;
	return rhs <= lhs;
}

bool operator==(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
		return false;
	if (lhs.sign != rhs.sign || lhs.data.size() != rhs.data.size())
		return false;
	for (std::size_t i = lhs.data.size() - 1; i > 0; i--)
	{
		if (rhs.data[i] != lhs.data[i])
			return false;
	}
	return lhs.data[0] == rhs.data[0];
}

bool operator!=(const LN &lhs, const LN &rhs)
{
	return !(lhs == rhs);
}

std::pair< std::uint32_t, std::uint32_t > calc_rank(std::uint32_t l, std::uint32_t r, std::uint32_t u_r)
{
	std::uint32_t l_rank = l + r + u_r;
	std::uint64_t u_rank =
		static_cast< std::uint64_t >(l) + static_cast< std::uint64_t >(r) + static_cast< std::uint64_t >(u_r) - l_rank;
	u_rank = u_rank >> 32;
	return { l_rank, static_cast< std::uint32_t >(u_rank) };
}

LN LN::sum(const LN &lhs, const LN &rhs)
{
	std::size_t min_size = std::min(lhs.data.size(), rhs.data.size());
	std::size_t max_size = std::max(lhs.data.size(), rhs.data.size());
	std::vector< std::uint32_t > data(max_size);
	std::uint32_t u_rank = 0;

	for (std::size_t i = 0; i < min_size; i++)
	{
		auto pair_ranks = calc_rank(lhs.data[i], rhs.data[i], u_rank);
		data[i] = pair_ranks.first;
		u_rank = pair_ranks.second;
	}
	if (max_size == lhs.data.size())
	{
		for (std::size_t i = min_size; i < max_size; i++)
		{
			auto pair_ranks = calc_rank(lhs.data[i], 0, u_rank);
			data[i] = pair_ranks.first;
			u_rank = pair_ranks.second;
		}
	}
	else
	{
		for (std::size_t i = min_size; i < max_size; i++)
		{
			auto pair_ranks = calc_rank(0, rhs.data[i], u_rank);
			data[i] = pair_ranks.first;
			u_rank = pair_ranks.second;
		}
	}
	if (u_rank)
	{
		data.push_back(u_rank);
	}
	return LN(lhs.sign, std::move(data));
}

LN LN::sub(const LN &lhs, const LN &rhs)
{
	std::size_t min_size = std::min(lhs.data.size(), rhs.data.size());
	std::vector< std::uint32_t > data(std::max(lhs.data.size(), rhs.data.size()));
	std::uint32_t u_rank = 0;

	for (std::size_t i = 0; i < min_size; i++)
	{
		data[i] = lhs.data[i] - rhs.data[i] - u_rank;
		if (lhs.data[i] < (rhs.data[i] + u_rank) || ((rhs.data[i] + 1u) == 0 && u_rank))
		{
			u_rank = 1;
		}
		else
		{
			u_rank = 0;
		}
	}
	for (std::size_t i = min_size; i < lhs.data.size(); i++)
	{
		data[i] = lhs.data[i] - u_rank;
		if (lhs.data[i] < u_rank)
		{
			u_rank = 1;
		}
		else
		{
			u_rank = 0;
		}
	}
	return LN(lhs.sign, std::move(data));
	;
}

LN operator+(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	if (lhs.sign != rhs.sign)
	{
		if (LN::modul_less(lhs, rhs))
		{
			return LN::sub(rhs, lhs);
		}
		return LN::sub(lhs, rhs);
	}
	return LN::sum(lhs, rhs);
}

LN &operator+=(LN &lhs, const LN &rhs)
{
	lhs = lhs + rhs;
	return lhs;
}

LN operator-(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	if (lhs.sign != rhs.sign)
	{
		return LN::sum(lhs, rhs);
	}
	if (LN::modul_less(lhs, rhs))
	{
		LN tmp = LN::sub(rhs, lhs);
		tmp.sign = !tmp.sign;
		return tmp;
	}
	return LN::sub(lhs, rhs);
}

LN &operator-=(LN &lhs, const LN &rhs)
{
	lhs = lhs - rhs;
	return lhs;
}

LN &LN::operator-()
{
	sign ^= 1;
	this->normalize();
	return *this;
}

LN &LN::operator~()
{
	if (isNaN() || sign)
	{
		NaN = 1;
		return *this;
	}
	LN tmp = *this;
	LN res;
	do
	{
		res = std::move(tmp);
		tmp = res + (*this / res);
		tmp.shift_one_bit_right();
	} while (tmp < res);
	return *this = std::move(res);
}

LN::operator long long() const
{
	if (isNaN())
		throw std::range_error("This Big Int is NaN");
	if (data.size() > 2)
		throw std::overflow_error("Big Int bigger than long long");
	std::uint32_t magic_num = 2147483647;
	if (magic_num < data[1] && !sign)
		throw std::overflow_error("Big Int bigger than long long");
	if (sign && data[1] == magic_num + sign && data[0] != 0)
		throw std::overflow_error("Big Int bigger than long long");
	long long res = -static_cast< long long >(data[0]) - (static_cast< long long >(data[1]) << 32);
	if (!sign)
		res = -res;
	return res;
}

LN::operator bool() const
{
	if (isNaN())
		return true;
	if (data.size() > 1)
		return true;
	return data[0] != 0;
}

LN operator<<(LN &&lhs, const std::size_t bits)
{
	if (lhs.isNaN())
	{
		lhs.data.clear();
		lhs.NaN = 1;
		return std::move(lhs);
	}
	std::size_t max_ind = lhs.get_normalized_ind();
	lhs.data.resize(bits + max_ind + 1);
	std::copy(lhs.data.rbegin() + bits, lhs.data.rend(), lhs.data.rbegin());
	std::fill(lhs.data.begin(), lhs.data.begin() + bits, 0);
	lhs.normalize();
	return std::move(lhs);
}

LN operator<<(const LN &lhs, const std::size_t bits)
{
	if (lhs.isNaN())
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	std::size_t max_ind = lhs.get_normalized_ind();
	std::vector< std::uint32_t > data(bits + max_ind + 1);
	std::copy(lhs.data.rbegin() + (lhs.data.size() - max_ind - 1), lhs.data.rend(), data.rbegin());
	std::fill(data.begin(), data.begin() + bits, 0);
	return LN(lhs.sign, std::move(data));
}

void LN::add_at_pos(LN &num, std::uint32_t u_rank, std::size_t pos)
{
	if (pos >= num.data.size())
		num.data.resize(pos + 1);
	for (std::size_t i = pos; i < num.data.size(); i++)
	{
		auto pair_ranks = calc_rank(num.data[i], 0, u_rank);
		num.data[i] = pair_ranks.first;
		u_rank = pair_ranks.second;
		if (u_rank == 0)
			return;
	}
	if (u_rank)
	{
		num.data.push_back(u_rank);
	}
	return;
}

LN LN::mul_uint(const LN &lhs, const std::uint32_t rhs)
{
	if (lhs.isNaN())
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	LN data_p;
	std::vector< std::uint32_t > data(lhs.data.size());

	for (std::size_t i = 0; i < lhs.data.size(); i++)
	{
		std::uint64_t tmp = static_cast< std::uint64_t >(lhs.data[i]) * static_cast< std::uint64_t >(rhs);
		data[i] = static_cast< std::uint32_t >(tmp);
		LN::add_at_pos(data_p, static_cast< std::uint32_t >(tmp >> 32), i + 1);
	}
	return data_p + LN(0, std::move(data));
}

LN operator*(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN())
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	LN res;
	for (std::size_t i = 0; i < rhs.data.size(); i++)
	{
		res = res + (LN::mul_uint(lhs, rhs.data[i]) << i);
	}
	res.sign = lhs.sign ^ rhs.sign;
	res.normalize();
	return res;
}

LN &operator*=(LN &lhs, const LN &rhs)
{
	lhs = lhs * rhs;
	return lhs;
}

inline bool LN::get_bit(std::size_t ind, std::uint32_t num)
{
	return num & (1 << ind);
}

inline void LN::set_bit(std::size_t ind, std::uint32_t &num, bool value)
{
	if (value)
		num |= (1 << ind);
	else
		num &= ~(1 << ind);
}

void LN::shift_one_bit()
{
	bool bit = 0;
	for (std::size_t i = 0; i < data.size(); i++)
	{
		bool new_bit = LN::get_bit(31, data[i]);
		data[i] = data[i] << 1;
		LN::set_bit(0, data[i], bit);
		bit = new_bit;
	}
	if (bit)
		data.push_back(1);
}

void LN::shift_one_bit_right()
{
	bool bit = 0;
	for (std::size_t i = data.size() - 1; i > 0; i--)
	{
		bool new_bit = LN::get_bit(0, data[i]);
		data[i] = data[i] >> 1;
		LN::set_bit(31, data[i], bit);
		bit = new_bit;
	}
	data[0] = data[0] >> 1;
	LN::set_bit(31, data[0], bit);
	normalize();
}

std::pair< LN, LN > LN::div_QR(const LN &lhs, const LN &rhs)
{
	LN Q;
	LN R;
	LN rhs_u = rhs;
	rhs_u.sign = 0;
	for (std::ptrdiff_t i = lhs.data.size() - 1; i >= 0; i--)
	{
		for (std::ptrdiff_t j = 31; j >= 0; j--)
		{
			R.shift_one_bit();
			LN::set_bit(0, R.data[0], LN::get_bit(j, lhs.data[i]));
			if (R >= rhs_u)
			{
				R -= rhs_u;
				if (Q.data.size() <= i)
					Q.data.resize(i + 1);
				LN::set_bit(j, Q.data[i], 1);
				Q.normalize();
			}
		}
	}
	Q.sign = lhs.sign ^ rhs.sign;
	Q.normalize();
	R.sign = lhs.sign;
	R.normalize();
	return std::make_pair(std::move(Q), std::move(R));
}

LN operator/(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN() || !rhs)
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	return LN::div_QR(lhs, rhs).first;
}

LN &operator/=(LN &lhs, const LN &rhs)
{
	lhs = lhs / rhs;
	return lhs;
}

LN operator%(const LN &lhs, const LN &rhs)
{
	if (lhs.isNaN() || rhs.isNaN() || !rhs)
	{
		LN res;
		res.NaN = 1;
		return res;
	}
	return LN::div_QR(lhs, rhs).second;
}

LN &operator%=(LN &lhs, const LN &rhs)
{
	lhs = lhs / rhs;
	return lhs;
}

std::istream &operator>>(std::istream &input, LN &num)
{
	std::string str;
	input >> str;
	num = LN(str);
	return input;
}

std::ostream &operator<<(std::ostream &out, const LN &num)
{
	if (num.isNaN())
	{
		out << "NaN";
		return out;
	}
	std::string res;
	LN tmp = num;
	LN div(1'000'000'000LL);
	while (tmp.data.size() > 1)
	{
		auto tmp_QR = LN::div_QR(tmp, div);
		tmp = std::move(tmp_QR.first);
		for (std::size_t i = 0; i < 9; i++)
		{
			res += tmp_QR.second.data[0] % 10 + '0';
			tmp_QR.second.data[0] /= 10;
		}
	}
	do
	{
		res += tmp.data[0] % 10 + '0';
		tmp.data[0] /= 10;
	} while (tmp.data[0] != 0);
	if (tmp.sign)
		res += "-";
	std::reverse(res.begin(), res.end());
	out << res;
	return out;
}

bool LN::isNaN() const
{
	return NaN;
}

LN operator"" _ln(const char *num)
{
	return LN(num);
}
