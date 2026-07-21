// IntrusiveForest.hpp
#pragma once

// External headers
#include "IntegralProxy/IntegralProxy.hpp"

// Implementation-specific headers
#include "IntrusiveForest/Traits.hpp"
#include "IntrusiveForest/Node.hpp"
#include "IntrusiveForest/TraversalPolicy.hpp"
#include "IntrusiveForest/NodeRef.hpp"
#include "IntrusiveForest/DepthMixin.hpp"
#include "IntrusiveForest/IteratorVerifier.hpp"
#include "IntrusiveForest/Iterator.hpp"
#include "IntrusiveForest/Container.hpp"



namespace cst
{

	// =======================================================================
	//  IntrusiveForest - node-based forest-like container
	// =======================================================================
	template <typename T, typename TTraits = cst::detail::IntrusiveForest::Traits<T>>
	using IntrusiveForest = cst::detail::IntrusiveForest::Container<T, TTraits>;

}  // namespace cst



