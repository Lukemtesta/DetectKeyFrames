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

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <boost/property_tree/ptree.hpp>

#include <iostream>

#include <memory>

namespace pipeline
{
    typedef boost::property_tree::ptree Tree;
    typedef std::shared_ptr<Tree> SharedTree;
}
 
namespace vision
{
    /// Struct used to tag regions between keyframes. Contains information about the descriptor cluster
    struct Tag
    {
        std::size_t frame_number;
        std::size_t frame_class;
        cv::Mat     descriptor;
        double      cluster_radius;
    };
}