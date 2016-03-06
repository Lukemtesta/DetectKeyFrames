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

#pragma once

#include "libdefs.h"

#include <boost/any.hpp>
#include <boost/optional.hpp>
#include <boost/circular_buffer.hpp>

#include <map>
#include <mutex>
#include <string>


namespace pipeline
{
    class Tray
    {
        public:
                /**
                * Parameterised constructor
                *
                *@param[in] buffer_capacity     Optional capacity for settings tray circular buffer size
                *
                */
                Tray(const std::size_t i_buffer_capacity = 0) : m_buffer_size(i_buffer_capacity) {};

                /**
                * Sets the variable associated with the tray key. 
                *
                *@param[in] i_key   Key for trays hash
                *@param[in] i_data  Data to store. Advise user to store shared pointer.
                *
                */
                template <class T> void SetVar(std::string i_key, const T & i_data)
                {
                    m_tray[i_key] = boost::any_cast<T>(i_data);
                }

                /**
                * Sets the variable associated with the tray key.
                *
                *@param[in] i_key   Key for trays hash
                *
                * \return Boost optional for an arbitary data type.
                */
                template <class T> boost::optional<T> GetVar(std::string i_key)
                {
                    auto data = m_tray.find(i_key);

                    if (data != m_tray.end())
                    {
                        try
                        {
                            return boost::any_cast<T>(data->second);
                        }
                        catch (boost::bad_any_cast & exception)
                        {
                            std::cout << exception.what() << "\n" << std::endl;
                            throw("Bad cast when retrieving " + i_key + " from tray");
                        }
                    }
                }

                /**
                * Sets the end of the buffer associated with the tray key. If no buffer 
                * exists it creates one. Cannot explicitly remove data once on the buffer.
                *
                *@param[in] i_key   Key for trays hash
                *@param[in] i_data  Data to store. Advise user to store shared pointer.
                *
                */
                template <class T> void SetBufferVar(std::string i_key, const T & i_data)
                {
                    // Create new buffer if key is empty, otherwise push back onto existing buffer
                    if (m_tray.find[i_key] == m_tray.end())
                    {
                        auto buffer = CreateBuffer();
                        m_tray[i_key] = boost::any< boost::circular_buffer<T> >(buffer);
                    }

                    try
                    {
                        m_tray[i_key]->push_back(i_data);
                    }
                    catch (std::exception & exception)
                    {
                        throw("Error: Failed when storing a value in " + i_key + " buffer on the tray");
                    }
                }

                /**
                * Retrieves the end of the buffer associated with the tray key
                *
                *@param[in] i_key   Key for trays hash
                *
                * \return Arbitary data at the back of the buffer stored under the input key
                */
                template <class T> boost::optional<T> GetBufferVar(std::string i_key)
                {
                    if (buffer != m_tray.end())
                    {
                        try
                        {
                            return boost::any_cast<T>(buffer->back());
                        }
                        catch (boost::bad_any_cast & exception)
                        {
                            throw(std::string("Error: Bad cast when retrieving " + i_key + " from buffer in tray"));
                        }
                    }
                }

        private:

                /**
                * Creates a circular buffer and stores it in the tray.
                *
                *@param[in] i_key   Key for trays hash
                *
                * \return True if hash is free and buffer successfully created
                */
                template <class T> std::shared_ptr<boost::circular_buffer<T>> CreateBuffer(std::string i_key)
                {
                    return boost::circular_buffer<T>(m_buffer_capacity);
                }

                /// Map for storing arbitary datatypes
                std::map<std::string, boost::any> m_tray;

                /// Buffer capacity
                std::size_t m_buffer_size;
    };

    typedef std::shared_ptr<Tray> SharedTray;
}