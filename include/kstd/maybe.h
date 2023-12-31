#ifndef _KSTD__MAYBE_H__
#define _KSTD__MAYBE_H__

#include "utility.h"

namespace kstd {

template<typename T>
class Maybe {
public:
	constexpr Maybe() = default;
	constexpr Maybe(T&& val);
	template<typename ...Args> constexpr Maybe(Args&&... args);

	Maybe(const Maybe&) = delete;
	Maybe& operator=(const Maybe& rhs) = delete;

	constexpr Maybe(Maybe&& other);
	constexpr Maybe& operator=(Maybe&& rhs);
	constexpr ~Maybe();

	constexpr bool has_value() const;

	constexpr T& value();
	constexpr const T& value() const;

	constexpr T * operator->();
	constexpr const T * operator->() const;

	constexpr T& operator*();
	constexpr const T& operator*() const;
	constexpr explicit operator bool() const;

	constexpr void reset();
	template<typename ...Args> constexpr void emplace(Args&& ...args);

private:
	alignas(alignof(T)) char storage[sizeof(T)];
	bool value_present = false;
};

template<typename T> constexpr Maybe<T>::Maybe(T&& val) : value_present(true)
{
	value() = move(val);
}

template<typename T> template<typename ...Args> constexpr Maybe<T>::Maybe(Args&&... args)
{
	emplace(forward(args)...);
}

template<typename T> constexpr Maybe<T>::Maybe(Maybe&& other)
	: value_present(other.value_present)
{
	if (!other.value_present)
		return;
	value() = move(other.value());
}

template<typename T> constexpr Maybe<T>& Maybe<T>::operator=(Maybe&& rhs)
{
	if (this == &rhs)
		return *this;

	if (value_present)
		reset();
	value_present = rhs.value_present;
	if (value_present) {
		value() = move(rhs.value());
		rhs.value_present = false;
	}
	
	return *this;
}

template<typename T> constexpr Maybe<T>::~Maybe()
{
	reset();
}

template<typename T> inline constexpr T& Maybe<T>::value()
{
	return *(reinterpret_cast<T *>(storage));
}

template<typename T> inline T constexpr const& Maybe<T>::value() const
{
	return *(reinterpret_cast<const T *>(storage));
}

template<typename T> inline constexpr bool Maybe<T>::has_value() const
{
	return value_present;
}

template<typename T> inline constexpr T& Maybe<T>::operator*()
{
	return value();
}

template<typename T> inline constexpr const T& Maybe<T>::operator*() const
{
	return value();
}

template<typename T> inline constexpr T * Maybe<T>::operator->()
{
	return &value();
}

template<typename T> inline constexpr const T * Maybe<T>::operator->() const
{
	return &value();
}

template<typename T> constexpr inline Maybe<T>::operator bool() const
{
	return value_present;
}

template<typename T> inline constexpr void Maybe<T>::reset()
{
	if (has_value()) {
		value().~T();
		value_present = false;
	}
}

template<typename T> template<typename ...Args>
inline constexpr void Maybe<T>::emplace(Args&&... args)
{
	new (reinterpret_cast<T *>(storage)) T(forward(args)...);
}

}

#endif
