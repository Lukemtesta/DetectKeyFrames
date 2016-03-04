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
#include "stage.h"

#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <list>

namespace vision
{
    class ClassRegionStage : public pipeline::Stage
    {
    public:
        ClassRegionStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage);
        ~ClassRegionStage();

        /**
        * Executes the main operation of the stage.
        *
        *@pram[in] i_tray   Shared memory container
        *
        */
        bool Process(pipeline::SharedTray & i_tray) override;

    private:

        bool CompareRegions(const cv::Mat & i_img);
        vision::Tag CreateRegion(const cv::Mat & i_img);

        /// Key for input frame in tray
        std::string m_frame_key;

        /// Key for key frame in tray
        std::string m_keyframe_key;

        /// Key for tags in tray
        std::string m_tags_key;

        // ORB detector
        cv::OrbFeatureDetector m_detector;

        // Flann matcher for descriptor matching
        cv::FlannBasedMatcher m_matcher;

        /// Tagged regions
        std::list<vision::Tag> m_tags;

        std::size_t m_class_count;
        std::size_t m_frame_number;
    };
}