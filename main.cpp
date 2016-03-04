
#include "pipeline.h"

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/filesystem.hpp>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <chrono>
#include <thread>

int main(int argc, char ** argv)
{
    namespace po = boost::program_options;

    std::string arguments("Arguments --pipeline <link to pipeline xml> --settings <config JSON> --input <Parent directory of images>");
    std::string description("App to cluster regions between keyframes in a video. " + arguments);

    po::options_description desc(description);
    desc.add_options()
        ("pipeline,p", po::value<std::string>()->required())
        ("settings,s", po::value<std::string>()->required())
        ("input,i", po::value<std::string>()->required());

    po::variables_map vm;

    try
    {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        notify(vm);
    }
    catch (std::exception &e)
    {
        std::cout << "Error: Improper use " + arguments << std::endl;
        return 1;
    }

    auto time_start = std::chrono::high_resolution_clock::now();


    // Create pipeline stages using the stage order in the pipeline xml file
    pipeline::SharedTree pipeline_settings = std::make_shared<boost::property_tree::ptree>();
    pipeline::SharedTree stage_settings = std::make_shared<boost::property_tree::ptree>();

    std::string pipeline_file(vm["pipeline"].as<std::string>());
    std::string stage_settings_file(vm["settings"].as<std::string>());

    read_xml(pipeline_file, *pipeline_settings);
    read_json(stage_settings_file, *stage_settings);


    // Link the pipelines input image buffer to the output buffer of a producer
    std::shared_ptr<bool> terminate = std::make_shared<bool>(false);
    pipeline::SharedImageBuffer buffer = std::make_shared<pipeline::ImageBuffer>();
    buffer->set_capacity(1);

    pipeline::Pipeline application(pipeline_settings, stage_settings);
    application.RegisterSourceBuffer(buffer);

    // \todo create consumer task and producer task wrappers so pipeline can run independently

    // Execute pipeline on a consumer thread
    std::thread consumer(
        [](pipeline::Pipeline & i_pipeline, std::shared_ptr<bool> & i_terminate){ i_pipeline.Run(i_terminate); },
        application,
        terminate);


    // Use this thread as an image producer - Read images from disk and store in consumers buffer
    boost::filesystem::directory_iterator file_it(vm["input"].as<std::string>());

    int frames = 0;
    for (const auto & file : file_it)
    {
        if (boost::filesystem::is_regular_file(file.path()))
        {
            buffer->push_back( cv::imread(file.path().string()) );

            while (buffer->capacity() == buffer->size())
            {
                std::this_thread::sleep_for(std::chrono::seconds(1 / 25));
            }
            ++frames;
        }
    }


    // No more frames to read - terminate pipeline
    *terminate = true;
    consumer.join();


    // calculate execution time
    auto time_end = std::chrono::high_resolution_clock::now();
    long long time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(time_end - time_start).count();
    
    std::cout << "fps: " << (static_cast<float>(frames)* 1e9f) / static_cast<float>(time_ns) << "s" << std::endl;

	return 0;
}

