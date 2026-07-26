// Traits.hpp
#pragma once

// Standard library headers
#include <type_traits>



namespace cst::detail::Forest
{

	// =======================================================================
	//  Traits - intrusive forest node traits
	// =======================================================================
	template <typename TValue, typename TSize = std::size_t, typename TDiff = std::ptrdiff_t>
	struct Traits
	{
		using value_type        = TValue;
		using size_type         = TSize;
		using difference_type   = TDiff;
		using pointer           = value_type*;
		using const_pointer     = const value_type*;
		using reference         = value_type&;
		using const_reference   = const value_type&;

		using size_tag          = std::false_type;
		using child_count_tag   = std::false_type;
		using depth_tag         = std::false_type;

	};  // struct Traits

}  // namespace cst::detail::Forest



