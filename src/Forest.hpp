// Forest.hpp
#pragma once

// External headers
#include "IntegralProxy/IntegralProxy.hpp"

// Implementation-specific headers
#include "Forest/Traits.hpp"
#include "Forest/Node.hpp"
#include "Forest/TraversalPolicy.hpp"
#include "Forest/NodeRef.hpp"
#include "Forest/DepthMixin.hpp"
#include "Forest/IteratorVerifier.hpp"
#include "Forest/Iterator.hpp"
#include "Forest/Container.hpp"



namespace cst
{

	// =======================================================================
	//  ForestTraits - forest node traits
	// =======================================================================
	template <typename T>
	using ForestTraits = cst::detail::Forest::Traits<T>;



	// =======================================================================
	//  Forest - node-based forest-like container
	// =======================================================================
	template <typename T, typename TTraits = cst::detail::Forest::Traits<T>>
	using Forest = cst::detail::Forest::Container<T, TTraits>;

}  // namespace cst



