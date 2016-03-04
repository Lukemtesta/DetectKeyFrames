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

#include "image_histogram_stage.h"
#include "vision_utils.h"

#include "numerical.h"

#include "opencv2/imgproc/imgproc.hpp"

#include <algorithm>
#include <numeric>

namespace vision
{
    ImageHistogramStage::ImageHistogramStage(std::string i_stage_name, pipeline::SharedTree & i_tree, pipeline::SharedStage i_next_stage) :
        Stage(i_stage_name, i_tree, i_next_stage),
        m_frame_number(0),
        m_class_count(0)
    {
        auto & config = i_tree->find(i_stage_name)->second;

        m_frame_key = config.find("input_image")->second.get_value<std::string>();
        m_keyframe_key = config.find("input_keyframe")->second.get_value<std::string>();
        m_cells = config.find("partitions")->second.get_value<std::size_t>();
        m_bin_key = config.find("bins")->second.get_value<std::size_t>();
    }

    ImageHistogramStage::~ImageHistogramStage() {};

    bool ImageHistogramStage::Process(pipeline::SharedTray & i_tray)
    {
        auto img_ptr = i_tray->GetVar<std::shared_ptr<cv::Mat>>(m_frame_key);
        auto keyframe_ptr = i_tray->GetVar<bool>(m_keyframe_key);

        if (!img_ptr)
        {
            return false;
        }

        auto img = img_ptr->get()->clone();

        /// Establish the number of bins
        cv::Mat edge_histogram = EdgeDescriptor(img);
        ++m_frame_number;

        // End of sequence, cache histograms
        bool end = false;

        if (keyframe_ptr)
        {
            end = *keyframe_ptr;
        }

        if (end)
        {
            if (m_tags.size() == 0)
            {
                m_sequence.clear();
                m_sequence.push_back(edge_histogram);
                m_tags.push_back( CreateRegion(m_sequence) );
            }
            else
            {                
                // Compare regions. If fits region, inherit regions tag. If outlier in all regions create new region
                cv::Mat mean_hist = vision::MeanHistogram(m_sequence);

                if (!CompareRegions(m_tags, mean_hist))
                {
                    m_tags.push_back(CreateRegion(m_sequence));
                }

                m_sequence.clear();
            }
        }
        else
        {
            m_sequence.push_back(edge_histogram.clone());
        }

        i_tray->SetVar("tags", std::make_shared<std::list<Tag>>(m_tags));

        return true;
    }

    bool ImageHistogramStage::CompareRegions(std::list<Tag> & o_tags, const cv::Mat & i_hist)
    {
        bool ret = false;
        Tag region_tag;

        for (const auto & tag : o_tags)
        {
            double distance = cv::compareHist(tag.descriptor, i_hist, CV_COMP_BHATTACHARYYA);
            if (distance <= tag.cluster_radius)
            {
                ret = true;
                region_tag = tag;
                break;
            }
        }

        if (ret)
        {
            region_tag.frame_number = m_frame_number;
            o_tags.push_back(region_tag);
        }

        return ret;
    }

    

    Tag ImageHistogramStage::CreateRegion(const std::list<cv::Mat> & i_histograms)
    {
        cv::Mat mean_hist;

        // Calculate mean histogram 
        mean_hist = vision::MeanHistogram(i_histograms);

        // Compare all histograms to mean. Take limit of cluster at mean + 3std.
        std::list<double> ls;
        double extreme = std::numeric_limits<double>::max();

        for (const auto & hist : m_sequence)
        {
            double coeff = cv::compareHist(mean_hist, hist, CV_COMP_BHATTACHARYYA);
            ls.push_back(coeff);
            extreme = coeff < extreme ? coeff : extreme;
        }

        double mean = std::accumulate(ls.begin(), ls.end(), 0.0) / static_cast<double>(ls.size());

        Tag region_descriptor;
        region_descriptor.frame_class = m_class_count++;
        region_descriptor.frame_number = m_frame_number;
        region_descriptor.descriptor = mean_hist;
        region_descriptor.cluster_radius = mean + (extreme);

        return region_descriptor;
    }


    cv::Mat ImageHistogramStage::EdgeDescriptor(const cv::Mat & i_img)
    {
        cv::Mat descriptor;
        std::vector<cv::Rect> cell_masks = vision::UniformPartition(i_img.size(), cv::Size(m_cells, m_cells));

        std::vector<cv::Mat> hists;
        for (const auto & roi : cell_masks)
        {
            hists.push_back(vision::CalculateHistogram(i_img(roi), m_bin_key));
        }

        cv::vconcat(hists, descriptor);

        return descriptor;
    }
}