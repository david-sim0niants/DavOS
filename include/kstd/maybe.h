#ifndef _KSTD__MAYBE_H__
#define _KSTD__MAYBE_H__

#include "utility.h"

namespace kstd {

template<typename T>
class Maybe {
public:
	Maybe() = default;
	Maybe(T &&val);
	template<typename ...Args> Maybe(Args &&...args);

	Maybe(const Maybe &) = delete;
	Maybe &operator=(const Maybe &rhs) = delete;

	Maybe(Maybe &&other);
	Maybe &operator=(Maybe &&rhs);
	~Maybe();

	bool has_value() const;

	T &value();
	const T &value() const;

	T *operator->();
	const T *operator->() const;

	T &operator*();
	const T &operator*() const;
	explicit operator bool() const;

	void reset();
	template<typename ...Args> void emplace(Args &&...args);

private:
	alignas(alignof(T)) char storage[sizeof(T)];
	bool value_present = false;
};

template<typename T> Maybe<T>::Maybe(T &&val) : value_present(true)
{
	value() = move(val);
}

template<typename T> template<typename ...Args> Maybe<T>::Maybe(Args &&...args)
{
	emplace(forward(args)...);
}

template<typename T> Maybe<T>::Maybe(Maybe &&other)
	: value_present(other.value_present)
{
	if (!other.value_present)
		return;
	value() = move(other.value());
}

template<typename T> Maybe<T> &Maybe<T>::operator=(Maybe &&rhs)
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

template<typename T> Maybe<T>::~Maybe()
{
	reset();
}

template<typename T> inline T &Maybe<T>::value()
{
	return *(reinterpret_cast<T *>(storage));
}

template<typename T> inline T const &Maybe<T>::value() const
{
	return *(reinterpret_cast<const T *>(storage));
}

template<typename T> inline bool Maybe<T>::has_value() const
{
	return value_present;
}

template<typename T> inline T & Maybe<T>::operator*()
{
	return value();
}

template<typename T> inline const T & Maybe<T>::operator*() const
{
	return value();
}

template<typename T> inline T * Maybe<T>::operator->()
{
	return &value();
}

template<typename T> inline const T * Maybe<T>::operator->() const
{
	return &value();
}

template<typename T> inline Maybe<T>::operator bool() const
{
	return value_present;
}

template<typename T> inline void Maybe<T>::reset()
{
	if (has_value()) {
		value().~T();
		value_present = false;
	}
}

template<typename T> template<typename ...Args>
inline void Maybe<T>::emplace(Args &&...args)
{
	new (reinterpret_cast<T *>(storage)) T(forward(args)...);
}

}

#endif
