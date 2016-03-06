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

#include "tray.h"

namespace pipeline
{
    class Stage
    {
        public:
                Stage(std::string i_stage_name);
                virtual ~Stage();

                /**
                * Executes the stages as a wrapper to process. Contains protection for failing gracefully
                */
                virtual bool Run(SharedTray & i_tray);

        protected:

                /**
                * Arbitary function containing the bulk of the stages processing steps.
                *
                *@param[in]     i_tray  Memory container
                *
                * \return False if exits premature
                */
                virtual bool Process(SharedTray & i_tray);
        
        private:
                /// Stage name resembling that of the pipeline json file
                std::string m_stage_name;
    };

    typedef std::shared_ptr<Stage> SharedStage;
}