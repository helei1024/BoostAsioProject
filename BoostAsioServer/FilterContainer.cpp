#include "stdafx.h"
#include "FilterContainer.h"

namespace pingcuo{
	template <class Predicate, class Iterator>
	FilterContainer<Predicate, Iterator>::FilterContainer(Predicate p, Iterator begin, Iterator end) :m_begin(p, begin, end), m_end(p, end, end)
	{

	}
	template <class Predicate, class Iterator>
	FilterContainer<Predicate, Iterator>::~FilterContainer()
	{

	}
}