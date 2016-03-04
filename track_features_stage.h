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

namespace vision
{
    class TrackFeaturesStage : public pipeline::Stage
    {
    public:
        TrackFeaturesStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage);
        ~TrackFeaturesStage();

        bool Process(pipeline::SharedTray & i_tray) override;

    private:
        /// Key for input frame in config file
        std::string m_frame_key;

        /// Key for keyframe in config file
        std::string m_keyframe_key;

        /// Key for output tracks in config file
        std::string m_output_tracks_key;

        /// Key for tracks status in config file
        std::string m_output_track_status_key;

        /// Key for track error in config file
        std::string m_output_tracks_error_key;
 
        /// Last image
        cv::Mat m_last_frame;

        /// Features to track
        std::vector<cv::Point2f> m_last_tracks;

        /// Feature time to live
        std::vector<uchar> m_tracks_ttl;
    };
}