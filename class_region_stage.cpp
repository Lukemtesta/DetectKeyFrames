/*
*
* Copyright (C) 2016 Luke Marcus Biagio Testa
*
* All rights reserved.  This software is protected by copyright
* law and international treaties.  No part of this software / document
* may be reproduced or distributed in any form or by any means,
* whether transiently or incidentally to some other use of this software,
* without the written permission of the copyright owner.
*
*/

#pragma once

#include "class_region_stage.h"
#include "vision_utils.h"

#include <numeric>

namespace vision
{
    ClassRegionStage::ClassRegionStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage),
        m_matcher(cv::FlannBasedMatcher(new cv::flann::LshIndexParams(20, 10, 2))),
        m_class_count(0),
        m_frame_number(0)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_frame_key = config.find("input_image")->second.get_value<std::string>();
        m_keyframe_key = config.find("input_keyframe")->second.get_value<std::string>();
        m_tags_key = config.find("output_tags")->second.get_value<std::string>();

        m_detector = cv::OrbFeatureDetector(200, 1.1f, 8, 31, 0, 2, cv::ORB::HARRIS_SCORE, 32);
    }

    ClassRegionStage::~ClassRegionStage() {};

    bool ClassRegionStage::Process(pipeline::SharedTray & i_tray)
    {
        auto img_ptr = i_tray->GetVar<std::shared_ptr<cv::Mat>>(m_frame_key);
        auto keyframe_ptr = i_tray->GetVar<bool>(m_keyframe_key);

        if (!img_ptr)
        {
            return false;
        }

        auto img = *(img_ptr->get());
        ++m_frame_number;

        if (*keyframe_ptr)
        {
            if (m_tags.size() == 0)
            {
                m_tags.push_back(CreateRegion(img));
            }
            else
            {
                if (!CompareRegions(img))
                {
                    m_tags.push_back(CreateRegion(img));
                }
            }
        }

        i_tray->SetVar(m_tags_key, std::make_shared<std::list<Tag>>(m_tags));

        return true;
    }

    Tag ClassRegionStage::CreateRegion(const cv::Mat & i_img)
    {
        cv::Mat descriptors;
        std::vector<cv::KeyPoint> keypoints;

        m_detector.detect(i_img, keypoints);
        m_detector.compute(i_img, keypoints, descriptors);

        Tag region_descriptor;
        region_descriptor.frame_class = m_class_count++;
        region_descriptor.frame_number = m_frame_number;
        region_descriptor.descriptor = descriptors;
        region_descriptor.cluster_radius = 0;

        return region_descriptor;
    }

    bool ClassRegionStage::CompareRegions(const cv::Mat & i_img)
    {
        bool ret = false;

        cv::Mat descriptors;
        std::vector<cv::KeyPoint> keypoints;
        std::vector< cv::DMatch > matches;

        // Detect features
        m_detector.detect(i_img, keypoints);
        m_detector.compute(i_img, keypoints, descriptors);

        for (const auto & tag : m_tags)
        {
            m_matcher.match(tag.descriptor, descriptors, matches);

            double mean_dist = 0.0;
            for (int i = 0; i < descriptors.rows; i++)
            {
                mean_dist += matches[i].distance;
            }
            mean_dist /= static_cast<double>(matches.size());

            // Compare match error - All matches should have a low error
            if (mean_dist <= 45)
            {
                Tag new_tag = tag;
                new_tag.frame_number = m_frame_number;
                m_tags.push_back(tag);
                ret = true;
                break;
            }
        }

        return ret;
    }
}