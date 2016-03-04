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

#include "write_tags_stage.h"
#include "vision_utils.h"
#include "image_histogram_stage.h"

#include <fstream>

namespace vision
{
    WriteTagsStage::WriteTagsStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_tags_key = config.find("input_tags")->second.get_value<std::string>();
        m_output_filename_key = config.find("filename")->second.get_value<std::string>();
        m_keyframe_key = config.find("input_keyframe")->second.get_value<std::string>();
    }

    WriteTagsStage::~WriteTagsStage() {};

    bool WriteTagsStage::Process(pipeline::SharedTray & i_tray)
    {
        auto tags_ptr = i_tray->GetVar<std::shared_ptr<std::list<Tag>>>(m_tags_key);
        auto keyframe_ptr = i_tray->GetVar<bool>(m_keyframe_key);

        if (!tags_ptr)
        {
            return false;
        }

        auto tags = *(tags_ptr->get());

        if (keyframe_ptr)
        {
            if (*keyframe_ptr)
            {
                std::ofstream ofs(m_output_filename_key);
                ofs << "keyframe , scene id\n";

                for (const auto & tag : tags)
                {
                    ofs << tag.frame_number << ", " << static_cast<char>(tag.frame_class + 65) << std::endl;
                }

                ofs.close();
            }
        }

        return true;
    }
}