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

#include "libdefs.h"

#include <list>

namespace vision
{
    /**
    * Up or downsample image by an arbitary factor
    */
    void ResizeImage(cv::Mat & i_mat, const double x = 0.5, const double y = 0.5);

    /**
    * Compute image derivatives using Sobel operators
    */
    void ImageDerivatives(cv::Mat & i_img, cv::Mat & o_img);

    /**
    * Detect corners in an image using Shi & Thomani's corner detector
    */
    void DetectCorners(
        std::vector<cv::Point2f> & o_pts,
        std::vector<uchar> & o_ttl,
        const cv::Mat & i_img,
        const double i_quality = 0.01,
        const double i_min_distance = 10,
        const int i_block_size = 3,
        const double i_max_corners = 200,
        const double i_corner_coefficient = 0.04,
        const cv::Size i_sub_pixel_size = cv::Size(10, 10),
        const cv::Size i_win_size = cv::Size(21, 21),
        const cv::Size i_zero_size = cv::Size(-1, -1));

    /**
    * Track keypoints between frames with optical flow
    */
    void TrackCorners(
        std::vector<cv::Point2f> & o_pts,
        std::vector<uchar> & o_ttl,
        std::vector<float> & o_err,
        std::vector<cv::Point2f> & i_last_pts,
        const cv::Mat & i_img,
        const cv::Mat & i_last_img,
        const cv::Size i_win_size = cv::Size(21,21));

    /**
    * Extract rects of equal dimensions representing a scene uniformly partitioned into N x N cells 
    */
    std::vector<cv::Rect> UniformPartition(const cv::Size i_img_size, const cv::Size i_partitions);

    cv::Mat CalculateHistogram(
        const cv::Mat & i_img,
        int i_bins = 8,
        float i_min = 0,
        float i_max = 256);

    /**
    * Compute the mean matrix from a list of matrices with equal dimensions
    */
    cv::Mat MeanHistogram(const std::list<cv::Mat> & i_hists);
}