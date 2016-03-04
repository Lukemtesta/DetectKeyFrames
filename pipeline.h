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

#include "greyscale_conversion_stage.h"
#include "smooth_image_stage.h"
#include "image_equalisation_stage.h"
#include "resize_image_stage.h"
#include "normalise_image_stage.h"
#include "gradient_image_stage.h"
#include "image_histogram_stage.h"
#include "track_features_stage.h"
#include "process_tracks_stage.h"
#include "class_region_stage.h"
#include "write_tags_stage.h"

#include <boost/circular_buffer.hpp>

#include <list>


namespace
{
    struct StageCreatorBase
    {
        virtual ~StageCreatorBase() {};

        virtual pipeline::SharedStage CreateStage(
            std::string i_stage_name, 
            pipeline::SharedTree & i_tree, 
            pipeline::SharedStage i_next_stage = nullptr) = 0;
    };


    template <class T> struct StageCreator : public StageCreatorBase
    {
        pipeline::SharedStage CreateStage(
            std::string i_stage_name, 
            pipeline::SharedTree & i_tree,
            pipeline::SharedStage i_next_stage = nullptr)
        {
            return std::make_shared<T>(i_stage_name, i_tree, i_next_stage);
        }
    };


    template <class T> std::shared_ptr<StageCreatorBase> CreateStageCreator()
    {
        return std::make_shared<StageCreator<T>>();
    }


    const std::map< std::string, std::shared_ptr<StageCreatorBase> > m_stage_definitions
    {
        { "GreyscaleConvertStage",  CreateStageCreator<vision::GreyscaleConvertStage>() },
        { "ResizeImageStage",       CreateStageCreator<vision::ResizeImageStage>() },
        { "NormaliseImageStage",    CreateStageCreator<vision::NormaliseImageStage>() },
        { "GradientImageStage",     CreateStageCreator<vision::GradientImageStage>() },
        { "ImageHistogramStage",    CreateStageCreator<vision::ImageHistogramStage>() },
        { "TrackFeaturesStage",     CreateStageCreator<vision::TrackFeaturesStage>() },
        { "ProcessTracksStage",     CreateStageCreator<vision::ProcessTracksStage>() },
        { "ImageEqualisationStage", CreateStageCreator<vision::ImageEqualisationStage>() },
        { "SmoothImageStage",       CreateStageCreator<vision::SmoothImageStage>() },
        { "WriteTagsStage",         CreateStageCreator<vision::WriteTagsStage>() },
        { "ClassRegionStage",       CreateStageCreator<vision::ClassRegionStage>() }
    };
}


namespace pipeline
{
    typedef std::list<SharedStage> SharedStages;
    typedef boost::circular_buffer<cv::Mat> ImageBuffer;
    typedef std::shared_ptr<boost::circular_buffer<cv::Mat>> SharedImageBuffer;

    class Pipeline
    {
        public:
                /**
                * Parameterised constructor
                *
                * @param[in]    i_pipeline_xml  Pipeline xml in a property tree
                * @param[in]    i_pipeline_json Pipeline stages json configuration file in a property tree
                */
                Pipeline(SharedTree & i_pipeline_xml, SharedTree & i_settings_json);

                /**
                * Executes the pipeline. Pipeline only runs if the source extracts data from the 
                * buffer shared between the producer (external) and consumer (source) for now 
                * \todo create producer class and consumer class interfaces to remove this limitation
                *
                * @param[in] i_terminate    True to request termination
                * @param[in] i_output_key   Tray key to store consumed images against
                *
                * \return False if pipeline exits prematurely due to a stage crashing
                */
                bool Run(std::shared_ptr<bool> & i_terminate, std::string i_output_key = "source");

                /**
                * Register an external buffer for the pipelines consumer
                */
                void RegisterSourceBuffer(SharedImageBuffer & i_source_buffer);

        private:
                /**
                * Instantiates stages and stores instances in a linked list
                *
                * @param[in]    i_pipeline_xml  Pipeline xml in a property tree
                * @param[in]    i_pipeline_json Pipeline stages json configuration file in a property tree
                */
                void CreatePipeline(SharedTree & i_pipeline_xml, SharedTree & i_settings_json);

                /**
                * Instantiates parallel or serial sub-pipeline stages and stores in a linked list.
                * Lowest level is a parallel stage (cannot treat a parallel stage as a master thread).
                *
                * @param[in]    i_pipeline_xml  Pipeline xml in a property tree
                * @param[in]    i_pipeline_json Pipeline stages json configuration file in a property tree
                */
                void CreatePipeline(Tree & i_pipeline_xml, SharedTree & i_settings_json);

                /// Shared memory container for pipeline stages
                SharedTray  m_tray;

                /// Linked list of sub-pipelines to execute.
                std::list<SharedStages> m_pipeline;

                /// Buffer shared between producer and consumer (pipeline)
                SharedImageBuffer       m_source_buffer;
    };
}

