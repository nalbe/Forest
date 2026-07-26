// IntegralProxy.hpp
#pragma once

// Standard library headers
#include <type_traits>



// IntegralProxy
namespace cst
{

	// =======================================================================
	//  IntegralProxy - proxy interface for a integral type
	// =======================================================================
	template <typename TValue, typename TBool = std::true_type>
	class IntegralProxy;

}  // namespace cst



/// IntegralProxy<value_type, std::true_type>
namespace cst
{

	// -------------------------------------------------------------
	//  IntegralProxy true type specialization
	// -------------------------------------------------------------
	template <typename TValue>
	class IntegralProxy<TValue, std::true_type>
	{
		static_assert(std::is_integral_v<TValue>,
			"IntegralProxy requires an integral type.");

		// -- type aliases -------------------------------------------------------
	private:
		using self_type   = IntegralProxy<TValue, std::true_type>;
		using value_type  = TValue;

		// -- members ------------------------------------------------------------
	private:
		value_type& m_value;

		// -- non-modifiers ------------------------------------------------------
	public:
		[[nodiscard]] value_type get()                  const noexcept;
		operator value_type()                           const noexcept;

		// -- modifiers ----------------------------------------------------------
	public:
		void set(value_type)                                  noexcept;
		void reset()                                          noexcept;

		// -- operators ----------------------------------------------------------
	public:
		self_type& operator=(const IntegralProxy&)            noexcept;
		self_type& operator=(value_type)                      noexcept;

		self_type& operator+=(value_type)                     noexcept;
		self_type& operator-=(value_type)                     noexcept;
		self_type& operator*=(value_type)                     noexcept;
		self_type& operator/=(value_type)                     noexcept;
		self_type& operator%=(value_type)                     noexcept;

		self_type& operator++()                               noexcept;
		value_type operator++(int)                            noexcept;
		self_type& operator--()                               noexcept;
		value_type operator--(int)                            noexcept;

		value_type operator-()                          const noexcept;

		// -- lifecycle ----------------------------------------------------------
	public:
		explicit IntegralProxy(value_type&)                   noexcept;

	};  // class IntegralProxy<value_type, std::true_type>



	/// -- non-modifiers ------------------------------------------------------

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::get() const noexcept -> value_type
	{
		return m_value;
	}

	template <typename TValue>
	IntegralProxy<TValue, std::true_type>::operator value_type() const noexcept
	{
		return m_value;
	}


	/// -- modifiers ----------------------------------------------------------

	template <typename TValue>
	void IntegralProxy<TValue, std::true_type>::set(value_type value) noexcept
	{
		m_value = value;
	}

	template <typename TValue>
	void IntegralProxy<TValue, std::true_type>::reset() noexcept
	{
		m_value = {};
	}


	/// -- operators ----------------------------------------------------------

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator=(const IntegralProxy& other) noexcept -> self_type&
	{
		m_value = other.m_value;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator=(value_type value) noexcept -> self_type&
	{
		m_value = value;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator+=(value_type delta) noexcept -> self_type&
	{
		m_value += delta;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator-=(value_type delta) noexcept -> self_type&
	{
		m_value -= delta;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator*=(value_type factor) noexcept -> self_type&
	{
		m_value *= factor;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator/=(value_type divisor) noexcept -> self_type&
	{
		m_value /= divisor;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator%=(value_type divisor) noexcept -> self_type&
	{
		m_value %= divisor;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator++() noexcept -> self_type&
	{
		++m_value;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator++(int) noexcept -> value_type
	{
		value_type old = m_value;
		++m_value;
		return old;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator--() noexcept -> self_type&
	{
		--m_value;
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator--(int) noexcept -> value_type
	{
		value_type old = m_value;
		--m_value;
		return old;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::true_type>::operator-() const noexcept -> value_type
	{
		return -m_value;
	}


	/// -- lifecycle ----------------------------------------------------------

	template <typename TValue>
	IntegralProxy<TValue, std::true_type>::IntegralProxy(value_type& value) noexcept :
		m_value{ value }
	{}


}  // namespace cst



/// IntegralProxy<value_type, std::false_type>
namespace cst
{

	// -------------------------------------------------------------
	//  IntegralProxy false type specialization
	// -------------------------------------------------------------
	template <typename TValue>
	class IntegralProxy<TValue, std::false_type>
	{
	private:
		using self_type   = IntegralProxy<TValue, std::false_type>;
		using value_type  = TValue;

	private:
		const value_type m_value;

	public:
		value_type get()                                    const noexcept;
		operator value_type()                               const noexcept;

	public:
		void set([[maybe_unused]] value_type)                     noexcept;
		void reset()                                              noexcept;

	public:
		self_type& operator=(const IntegralProxy&)                noexcept;
		self_type& operator=([[maybe_unused]] value_type)         noexcept;

		self_type& operator+=([[maybe_unused]] value_type)        noexcept;
		self_type& operator-=([[maybe_unused]] value_type)        noexcept;
		self_type& operator*=([[maybe_unused]] value_type)        noexcept;
		self_type& operator/=([[maybe_unused]] value_type)        noexcept;
		self_type& operator%=([[maybe_unused]] value_type)        noexcept;

		self_type& operator++()                                   noexcept;
		value_type operator++(int)                                noexcept;
		self_type& operator--()                                   noexcept;
		value_type operator--(int)                                noexcept;

		value_type operator-()                              const noexcept;

	public:
		explicit IntegralProxy(const value_type&)                 noexcept;

	};  // class IntegralProxy<value_type, std::false_type>



	template <typename TValue>
	IntegralProxy<TValue, std::false_type>::IntegralProxy(const value_type& value) noexcept :
		m_value{ value }
	{}

	template <typename TValue>
	void IntegralProxy<TValue, std::false_type>::set(value_type) noexcept
	{}

	template <typename TValue>
	void IntegralProxy<TValue, std::false_type>::reset() noexcept
	{}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::get() const noexcept -> value_type
	{
		return m_value;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator=(const IntegralProxy&) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	IntegralProxy<TValue, std::false_type>::operator value_type() const noexcept
	{
		return m_value;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator=(value_type) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator+=(value_type) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator-=(value_type) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator*=(value_type) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator/=(value_type) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator%=(value_type) noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator++() noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator++(int) noexcept -> value_type
	{
		return m_value;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator--() noexcept -> self_type&
	{
		return *this;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator--(int) noexcept -> value_type
	{
		return m_value;
	}

	template <typename TValue>
	auto IntegralProxy<TValue, std::false_type>::operator-() const noexcept -> value_type
	{
		return -m_value;
	}


}  // namespace cst



/// free-standing arithmetic operators
namespace cst
{

	template <typename TValue, typename TBool>
	TValue operator+(const IntegralProxy<TValue, TBool>& lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs.get() + rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator-(const IntegralProxy<TValue, TBool>& lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs.get() - rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator*(const IntegralProxy<TValue, TBool>& lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs.get() * rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator/(const IntegralProxy<TValue, TBool>& lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs.get() / rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator%(const IntegralProxy<TValue, TBool>& lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs.get() % rhs.get();
	}


	template <typename TValue, typename TBool>
	TValue operator+(const IntegralProxy<TValue, TBool>& lhs, TValue rhs) noexcept
	{
		return lhs.get() + rhs;
	}

	template <typename TValue, typename TBool>
	TValue operator+(TValue lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs + rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator-(const IntegralProxy<TValue, TBool>& lhs, TValue rhs) noexcept
	{
		return lhs.get() - rhs;
	}

	template <typename TValue, typename TBool>
	TValue operator-(TValue lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs - rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator*(const IntegralProxy<TValue, TBool>& lhs, TValue rhs) noexcept
	{
		return lhs.get() * rhs;
	}

	template <typename TValue, typename TBool>
	TValue operator*(TValue lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs * rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator/(const IntegralProxy<TValue, TBool>& lhs, TValue rhs) noexcept
	{
		return lhs.get() / rhs;
	}

	template <typename TValue, typename TBool>
	TValue operator/(TValue lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs / rhs.get();
	}

	template <typename TValue, typename TBool>
	TValue operator%(const IntegralProxy<TValue, TBool>& lhs, TValue rhs) noexcept
	{
		return lhs.get() % rhs;
	}

	template <typename TValue, typename TBool>
	TValue operator%(TValue lhs, const IntegralProxy<TValue, TBool>& rhs) noexcept
	{
		return lhs % rhs.get();
	}


}  // namespace cst



