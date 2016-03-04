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

#include "resize_image_stage.h"
#include "vision_utils.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace vision
{
    ResizeImageStage::ResizeImageStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_frame_key = config.find("input_image")->second.get_value<std::string>();
        m_factor = config.find("resize_factor")->second.get_value<double>();
    }

    ResizeImageStage::~ResizeImageStage() {};

    bool ResizeImageStage::Process(pipeline::SharedTray & i_tray)
    {
        auto img_ptr = i_tray->GetVar<std::shared_ptr<cv::Mat>>(m_frame_key);

        if (!img_ptr)
        {
            return false;
        }

        auto img = *(img_ptr->get());

        vision::ResizeImage(img, m_factor, m_factor);

        i_tray->SetVar(m_frame_key, std::make_shared<cv::Mat>(img));

        return true;
    }
}