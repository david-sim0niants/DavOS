#ifndef _KSTD__EITHER_H__
#define _KSTD__EITHER_H__

#include "type_traits.h"

namespace kstd {

template<typename L, typename R> class Either {
public:
	Either(L left_val);
	Either(R left_val);


	template<typename T, typename L1, typename R1>
	friend bool has_type(const Either<L1, R1>& either);

	template<typename T, typename L1, typename R1>
	friend const T& get(const Either<L1, R1>&);

	template<typename T, typename L1, typename R1>
	friend const T& try_get(const Either<L1, R1>&);


private:
	const L& get_L() const;
	const R& get_R() const;

	static constexpr auto max(auto a, auto b)
	{
		return a > b ? a : b;
	}

	static constexpr auto tot_size = max(sizeof(L), sizeof(R));
	static constexpr auto tot_alignment = max(alignof(L), alignof(R));

	alignas(tot_alignment) char storage[tot_size];
	char current_type;
};

template<typename L, typename R> Either<L, R>::Either(L l_val) : current_type(0)
{
	*reinterpret_cast<L *>(storage) = l_val;
}

template<typename L, typename R> Either<L, R>::Either(R r_val) : current_type(1)
{
	*reinterpret_cast<R *>(storage) = r_val;
}

template<typename L, typename R> inline const L& Either<L, R>::get_L() const
{
	return *reinterpret_cast<const L *>(storage);
}
template<typename L, typename R> inline const R& Either<L, R>::get_R() const
{
	return *reinterpret_cast<const R *>(storage);
}

template<typename T, typename L, typename R>
inline bool has_type(const Either<L, R>& either)
{
	if constexpr (types_match<L, T>())
		return either.current_type == 0;
	else if constexpr (types_match<R, T>())
		return either.current_type == 1;
	else
		static_assert(types_match<L, T>() || types_match<R, T>(),
				"Wrong type for Either. L != T and R != T");
}

template<typename T, typename L, typename R>
inline const T& get(const Either<L, R>& either)
{
	if constexpr (types_match<L, T>())
		return either.get_L();
	else if constexpr (types_match<R, T>())
		return either.get_R();
	else
		static_assert(types_match<L, T>() || types_match<R, T>(),
				"Wrong type for Either. L != T and R != T");
}

template<typename T, typename L, typename R>
inline const T *try_get(const Either<L, R>& either)
{
	if (has_type<T>(either))
		return &get<T>(either);
	else
		return nullptr;
}

}

#endif
