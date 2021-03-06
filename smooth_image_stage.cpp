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

#include "smooth_image_stage.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace vision
{
    SmoothImageStage::SmoothImageStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_frame_key = config.find("input_image")->second.get_value<std::string>();
        m_output_key = config.find("output_image")->second.get_value<std::string>();
        m_kernel_size = config.find("kernel_size")->second.get_value<std::size_t>();
        m_kernel_sigma = config.find("kernel_sigma")->second.get_value<double>();
    }

    SmoothImageStage::~SmoothImageStage() {};

    bool SmoothImageStage::Process(pipeline::SharedTray & i_tray)
    {
        auto img_ptr = i_tray->GetVar<std::shared_ptr<cv::Mat>>(m_frame_key);

        if (!img_ptr)
        {
            return false;
        }

        auto img = *(img_ptr->get());

        cv::GaussianBlur(img, img, cv::Size(m_kernel_size, m_kernel_size), m_kernel_sigma);

        i_tray->SetVar(m_output_key, std::make_shared<cv::Mat>(img));

        return true;
    }
}