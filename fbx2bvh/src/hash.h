#pragma once

#include <string>
#include <cstdint>

struct Hash
{
	static constexpr uint64_t DEFAULT_SEED = 61081;

	Hash& Add(bool b)
	{
		mValue = Concatenate(b ? 0 : 1);
		return *this;
	}

	Hash& Add(int32_t i)
	{
		mValue = Concatenate(i);
		return *this;
	}

	Hash& Add(uint32_t i)
	{
		mValue = Concatenate(i);
		return *this;
	}

	Hash& Add(float f)
	{
		return AddObjectBytes(f);
	}

	Hash& Add(const char* cStr)
	{
		auto i0 = reinterpret_cast<const uint8_t*>(cStr);
		return AddBytes(i0, i0 + strlen(cStr));
	}

	Hash& Add(const std::string& str)
	{
		auto i0 = reinterpret_cast<const uint8_t*>(str.c_str());
		return AddBytes(i0, i0 + str.size());
	}

	Hash& AddBytes(const uint8_t* i0, const uint8_t* i1)
	{
		while (i0 != i1)
		{
			mValue = Concatenate(*i0);
			++i0;
		}
		return *this;
	}

	template <typename T>
	Hash& AddObjectBytes(const T& value)
	{
		auto i0 = reinterpret_cast<const uint8_t*>(&value);
		auto i1 = i0 + sizeof(T);
		while (i0 != i1)
		{
			mValue = Concatenate(*i0);
			++i0;
		}
		return *this;
	}

	operator uint64_t() const
	{
		return mValue;
	}

private:
	uint64_t mValue = DEFAULT_SEED;

	uint64_t Concatenate(uint64_t value)
	{
		return mValue * 31 + value;
	}
};

