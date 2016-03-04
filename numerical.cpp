
#include "numerical.h"

#include <numeric>

namespace numerical
{
    double StandardDeviation(const std::list<double> & i_data)
    {
        double mean = std::accumulate(i_data.begin(), i_data.end(), 0.0) / static_cast<double>(i_data.size());

        double sum = 0;
        for (const auto & data : i_data)
        {
            sum += (data - mean) * (data - mean);
        }
        sum = sum / static_cast<double>(i_data.size());

        return std::sqrt(sum);
    }

    StandardDeviationFilter::StandardDeviationFilter(
        const std::size_t i_buffer_size, 
        const double i_weighting) :
        m_weighting(i_weighting)
    {
        m_buffer = std::make_unique<boost::circular_buffer<double>>(i_buffer_size);
    }

    void StandardDeviationFilter::Clear()
    {
        m_buffer->clear();
    }

    bool StandardDeviationFilter::Filter(const double i_data)
    {
        bool ret = true;

        if (m_buffer->size() != m_buffer->capacity())
        {
            m_buffer->push_back(i_data);
        }
        else
        {
            // Update energy threshold
            std::list<double> samples;
            for (circular_buffer_t::iterator it = m_buffer->begin(); it != m_buffer->end(); ++it)
            {
                samples.push_back(*it);
            }

            double std =    StandardDeviation(samples);
            double mean =   std::accumulate(samples.begin(), samples.end(), 0.0) / static_cast<double>(samples.size());

            if ( m_weighting * std >= std::abs(mean - i_data) )
            {
                m_buffer->push_back(i_data);
            }
            else
            {
                ret = false;
            }
        }

        return ret;
    }

    std::unique_ptr<boost::circular_buffer<double>> m_buffer;
}