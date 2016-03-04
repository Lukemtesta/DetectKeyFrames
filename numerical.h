/*
*
* Copyright (c) 2016 Luke Marcus Biagio Testa
* All rights reserved.
*
* Redistribution and use in source and binary forms are permitted
* provided that the above copyright notice and this paragraph are
* duplicated in all such forms and that any documentation,
* advertising materials, and other materials related to such
* distribution and use acknowledge that the software was developed
* by the Luke Marcus Biagio Testa. The name of the
* Luke Marcus Biagio Testa may not be used to endorse or promote products derived
* from this software without specific prior written permission.
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*/

#include "libdefs.h"

#include <boost/circular_buffer.hpp>

#include <list>

namespace numerical
{
    typedef  boost::circular_buffer<double> circular_buffer_t;

    double StandardDeviation(const std::list<double> & i_data);

    class StandardDeviationFilter
    {
    public:
        /***
        * Paramaterised Constructors
        *
        * @param[in] i_buffer_size  Capacity of the filters buffer
        * @param[in] i_weighting    Weighting of the standard deviation for the filter input threshold
        */
        StandardDeviationFilter(const std::size_t i_buffer_size = 5, const double i_weighting = 3.0);

        /**
        * Filters an input sample. The buffer is stored if the value passes or the buffer is not full
        */
        bool Filter(const double i_data);

        /**
        * Clear buffer
        */
        void Clear();

    private:

        /// Weighting for the standard deviation threshold
        double  m_weighting;

        /// Buffer for caching the last N successful samples
        std::unique_ptr<circular_buffer_t> m_buffer;
    };
}