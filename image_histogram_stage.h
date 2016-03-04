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

#include <list>
#include <vector>

namespace vision
{
    class ImageHistogramStage : public pipeline::Stage
    {
    public:
        ImageHistogramStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage);
        ~ImageHistogramStage();

        /**
        * Executes the main operation of the stage.
        *
        *@pram[in] i_tray   Shared memory container
        *
        */
        bool Process(pipeline::SharedTray & i_tray) override;

        /**
        *
        * Partitions an image into N cells and computes a concatenated discriptor of image derivatives.
        *
        *@param[in] i_img   Input image to create an edge descriptor of
        *
        *\return Concatenated histogram of image cells
        */
        cv::Mat EdgeDescriptor(const cv::Mat & i_img);

        /**
        * Assigns video frames to a region and computes the radius of the cluster.
        *
        * @param[in] i_histograms   Edge descriptors for each image in the region
        *
        * \return Regions tag
        */
        Tag CreateRegion(const std::list<cv::Mat> & i_histograms);

        /**
        * Computes the mean descriptor of a region and compares it to previous regions. 
        *
        * @param[out] o_histograms  Clusters for previously annotated regions
        * @param[in] i_hist         Mean descriptor of the region to compare
        *
        * \return Regions tag
        */
        bool CompareRegions(std::list<Tag> & o_histograms, const cv::Mat & i_hist);

        /// Key for input frame in tray
        std::string         m_frame_key;

        /// Key for output frame in tray
        std::string         m_output_frame_key;

        /// Key for keyframe in tray
        std::string         m_keyframe_key;

        /// Number of partitions along a plane
        std::size_t         m_cells;

        /// Histogram bin count
        std::size_t         m_bin_key;

        std::size_t         m_class_count;

        std::size_t         m_frame_number;
        std::list<cv::Mat>  m_sequence;
        std::list<Tag>      m_tags;
    };
}