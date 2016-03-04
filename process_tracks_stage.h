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

#include "numerical.h"

namespace vision
{
    class ProcessTracksStage : public pipeline::Stage
    {
    public:
        ProcessTracksStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage);
        ~ProcessTracksStage();

        bool Process(pipeline::SharedTray & i_tray) override;

    private:
        
        /// Key for input frame in tray
        std::string m_frame_key;

        /// Key for keyframe in tray
        std::string m_keyframe_key;

        /// Key for tracks status in tray
        std::string m_ttl_key;

        /// Key for tracks error in tray
        std::string m_error_key;

        /// Standard deviation filter for detecting key frames
        std::unique_ptr<numerical::StandardDeviationFilter> m_filter;
    };
}