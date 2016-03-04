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
    class SmoothImageStage : public pipeline::Stage
    {
    public:
        SmoothImageStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage);
        ~SmoothImageStage();

        /**
        * Executes the main operation of the stage.
        *
        *@pram[in] i_tray   Shared memory container
        *
        */
        bool Process(pipeline::SharedTray & i_tray) override;

    private:

        /// Key for input frame in config file
        std::string m_frame_key;

        /// Key for output frame in config file
        std::string m_output_key;

        /// Gaussian kernel size
        std::size_t m_kernel_size;

        /// Gaussian kernel sigma
        double      m_kernel_sigma;
    };
}