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

#include "track_features_stage.h"

#include "vision_utils.h"

namespace vision
{
    TrackFeaturesStage::TrackFeaturesStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_frame_key = config.find("input_image")->second.get_value<std::string>();
        m_keyframe_key = config.find("input_keyframe")->second.get_value<std::string>();
        m_output_tracks_key = config.find("output_tracks")->second.get_value<std::string>();
        m_output_track_status_key = config.find("output_track_ttl")->second.get_value<std::string>();
        m_output_tracks_error_key = config.find("output_track_error")->second.get_value<std::string>();
    }

    TrackFeaturesStage::~TrackFeaturesStage() {};

    bool TrackFeaturesStage::Process(pipeline::SharedTray & i_tray)
    {
        auto img_ptr = i_tray->GetVar<std::shared_ptr<cv::Mat>>(m_frame_key);
        auto keyframe_ptr = i_tray->GetVar<bool>(m_keyframe_key);

        if (!img_ptr)
        {
            return false;
        }

        bool keyframe = false;
        auto img = *(img_ptr->get());

        if (keyframe_ptr)
        {
            keyframe = *keyframe_ptr;
        }

        std::vector<cv::Point2f> new_tracks;
        std::vector<float> tracking_error;

        if (m_last_tracks.size() < 2 || keyframe)
        {
            vision::DetectCorners(new_tracks, m_tracks_ttl, img);
        }
        else
        {
            vision::TrackCorners(new_tracks, m_tracks_ttl, tracking_error, m_last_tracks, img, m_last_frame);
        }

        m_last_tracks = new_tracks;
        m_last_frame = img.clone();

        i_tray->SetVar(m_keyframe_key, keyframe);
        i_tray->SetVar(m_output_track_status_key, std::make_shared<std::vector<uchar>>(m_tracks_ttl));
        i_tray->SetVar(m_output_tracks_error_key, std::make_shared<std::vector<float>>(tracking_error));
        i_tray->SetVar(m_output_tracks_key, std::make_shared<std::vector<cv::Point2f>>(new_tracks));

        return true;
    }
}