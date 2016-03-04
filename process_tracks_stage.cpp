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

#include "process_tracks_stage.h"

#include <algorithm>
#include <numeric>

namespace vision
{
    ProcessTracksStage::ProcessTracksStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_frame_key = config.find("input_image")->second.get_value<std::string>();
        m_keyframe_key = config.find("output_keyframe")->second.get_value<std::string>();
        m_ttl_key = config.find("input_track_ttl")->second.get_value<std::string>();
        m_error_key = config.find("input_track_error")->second.get_value<std::string>();
        std::size_t buffer_capacity = config.find("filter_buffer_size")->second.get_value<std::size_t>();
        double weighting = config.find("weighting")->second.get_value<std::size_t>();

        m_filter = std::make_unique<numerical::StandardDeviationFilter>(buffer_capacity, weighting);
    }

    ProcessTracksStage::~ProcessTracksStage() {};

    bool ProcessTracksStage::Process(pipeline::SharedTray & i_tray)
    {
        auto img_ptr = i_tray->GetVar<std::shared_ptr<cv::Mat>>(m_frame_key);
        auto track_ttl_ptr = i_tray->GetVar<std::shared_ptr<std::vector<uchar>>>(m_ttl_key);
        auto track_error_ptr = i_tray->GetVar<std::shared_ptr<std::vector<float>>>(m_error_key);

        if (!track_ttl_ptr || !track_error_ptr)
        {
            return false;
        }

        auto img = *(img_ptr->get());
        auto track_ttl = *(track_ttl_ptr->get());
        auto track_error = *(track_error_ptr->get());

        // Increase tracks time to live
        auto total_ttl =        static_cast<double>(track_ttl.size()) 
                                / std::accumulate(track_ttl.begin(), track_ttl.end(), 0.0);
        auto average_error  =   std::accumulate(track_error.begin(), track_error.end(), 0.0)
                                / static_cast<double>(track_ttl.size());

        bool keyframe = false;
        if (!m_filter->Filter(total_ttl * average_error))
        {
            keyframe = true;
            m_filter->Clear();
        }

        /*std::cout << "Number of points: " << track_ttl.size() << ", Persistent Points: " << total_ttl
            << ", Average error " << average_error << ", keyframe: " << keyframe << std::endl;*/

        i_tray->SetVar(m_keyframe_key, keyframe);

        return true;
    }
}