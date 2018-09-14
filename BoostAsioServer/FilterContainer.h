#pragma once

#include <boost/iterator/filter_iterator.hpp>  

namespace pingcuo{
	template <class Predicate, class Iterator>
	class FilterContainer
	{
	public:
		typedef boost::filter_iterator<Predicate, Iterator> FilterIter;

		FilterContainer(Predicate p, Iterator begin, Iterator end);
		~FilterContainer();

		FilterIter begin() { return m_begin; }
		FilterIter end()   { return m_end; }
		int szie() {
			int i = 0;
			FilterIter fi = m_begin;
			while (fi != m_end)
			{
				++i;
				++fi;
			}

			return i;
		}

	private:
		FilterIter m_begin;
		FilterIter m_end;
	};
}