
#include "vision_utils.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/tracking.hpp>

namespace vision
{
    void ResizeImage(cv::Mat & i_mat, const double x, const double y)
    {
        const double x_factor = x * static_cast<double>(i_mat.cols);
        const double y_factor = y * static_cast<double>(i_mat.rows);

        cv::resize(i_mat, i_mat, cv::Size(static_cast<int>(x_factor), static_cast<int>(y_factor)));
    }    


    void SobelFilter(cv::Mat & o_img)
    {
        cv::Mat Gx;
        cv::Mat Gy;

        cv::Sobel(o_img, Gx, o_img.depth(), 1, 0, 5);
        cv::Sobel(o_img, Gy, o_img.depth(), 0, 1, 5);

        cv::convertScaleAbs(Gx, Gx);
        cv::convertScaleAbs(Gy, Gy);

        cv::addWeighted(Gx, 0.5, Gy, 0.5, 0, o_img);
    }


    void TrackCorners(
        std::vector<cv::Point2f> & o_pts,
        std::vector<uchar> & o_ttl,
        std::vector<float> & o_err,
        std::vector<cv::Point2f> & i_last_pts,
        const cv::Mat & i_img,
        const cv::Mat & i_last_img,
        const cv::Size i_win_size)
    {
        std::vector<uchar> status;

        cv::TermCriteria termcrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03);

        // Track corners between frames
        cv::calcOpticalFlowPyrLK(i_last_img,
            i_img,
            i_last_pts,
            o_pts,
            o_ttl,
            o_err,
            i_win_size,
            3,
            termcrit
            );

    }

    void DetectCorners(
        std::vector<cv::Point2f> & o_pts,
        std::vector<uchar> & o_ttl,
        const cv::Mat & i_img,
        const double i_quality,
        const double i_min_distance,
        const int i_block_size,
        const double i_max_corners,
        const double i_corner_coefficient,
        const cv::Size i_sub_pixel_size,
        const cv::Size i_win_size,
        const cv::Size i_zero_size)
    {
        // Detect shi & tomansis minimum eigenvalue corner points
        goodFeaturesToTrack(i_img,
            o_pts,
            i_max_corners,
            i_quality,
            i_min_distance,
            cv::Mat(),
            i_block_size,
            false,
            i_corner_coefficient);

        // Get subpixel position precision
        cv::TermCriteria criteria = cv::TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001);

        cv::cornerSubPix(i_img, o_pts, i_win_size, i_zero_size, criteria);

        // Initialise points time to live
        o_ttl.clear();
        o_ttl.reserve(i_max_corners);

        for (std::size_t i = 0; i < i_max_corners; ++i)
        {
            o_ttl.push_back(1);
        }
    }

    std::vector<cv::Rect> UniformPartition(const cv::Size i_img_size, const cv::Size i_partitions)
    {
        // parition mat into 8 cells
        std::size_t width_step = std::floor(static_cast<float>(i_img_size.width) / static_cast<float>(i_partitions.width));
        std::size_t height_step = std::floor(static_cast<float>(i_img_size.height) / static_cast<float>(i_partitions.height));
        std::vector<cv::Rect> cell_masks;

        for (std::size_t i = 0; i < i_partitions.width; ++i)
        {
            for (std::size_t j = 0; j < i_partitions.height; ++j)
            {
                cell_masks.push_back(
                    cv::Rect(
                    static_cast<int>(i * width_step), 
                    static_cast<int>(j * height_step), 
                    static_cast<int>(width_step),
                    static_cast<int>(height_step)));
            }
        }

        return cell_masks;
    }


    cv::Mat CalculateHistogram(
        const cv::Mat & i_img, 
        int i_bins, 
        float i_min, 
        float i_max)
    {
        cv::Mat o_hist;

        float range[] = { i_min, i_max };
        const float * histRange = { range };

        /// Compute the histograms:
        cv::calcHist(&i_img, 1, 0, cv::Mat(), o_hist, 1, &i_bins, &histRange, true, false);

        // Draw the histograms for B, G and R
        /*int hist_w = 512; int hist_h = 400;
        int bin_w = cvRound((double)hist_w / histSize);

        cv::Mat histImage(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

        /// Normalize the result to [ 0, histImage.rows ]
        normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());*/

        return o_hist;
    }


    cv::Mat MeanHistogram(const std::list<cv::Mat> & i_hists)
    {
        cv::Mat mean;
        const auto hist = i_hists.begin();

        mean = cv::Mat(hist->rows, hist->cols, hist->type(), cv::Scalar(0));
        for (const auto & hist : i_hists)
        {
            mean += hist;
        }
        mean /= static_cast<double>(i_hists.size());

        return mean;
    }
}