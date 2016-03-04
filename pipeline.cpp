
#include "pipeline.h"


namespace pipeline
{
    Pipeline::Pipeline(SharedTree & i_pipeline_xml, SharedTree & i_settings_json)
    {
        m_tray = std::make_shared<Tray>();
        CreatePipeline(i_pipeline_xml, i_settings_json);
    };
    

    void Pipeline::CreatePipeline(SharedTree & i_pipeline_xml, SharedTree & i_settings_json)
    {
        // Create a stage for each xml field 
        for (const auto & stage_config : *i_pipeline_xml)
        {
            if (stage_config.first == "Parallel")
            {
                CreatePipeline(i_pipeline_xml->get_child(stage_config.first), i_settings_json);
            }
            else
            {
                auto creator = m_stage_definitions.find(stage_config.first);

                if (creator != m_stage_definitions.end())
                {
                    // Create a linked list style pipeline through the base header 
                    auto stage = creator->second->CreateStage(stage_config.first, i_settings_json);

                    if (stage != nullptr)
                    {
                        m_pipeline.push_back({stage});
                    }
                }
            }
        }
    }

    void Pipeline::CreatePipeline(Tree & i_pipeline_xml, SharedTree & i_settings_json)
    {
        SharedStages pipeline;

        // Create a stage for each xml field 
        for (const auto & stage_config : i_pipeline_xml)
        {
            auto creator = m_stage_definitions.find(stage_config.first);

            if (creator != m_stage_definitions.end())
            {
                // Create a linked list style pipeline through the base header 
                auto stage = creator->second->CreateStage(stage_config.first, i_settings_json);

                if (stage != nullptr)
                {
                    pipeline.push_back(stage);
                }
            }
        }

        m_pipeline.push_back(pipeline);
    }

    void Pipeline::RegisterSourceBuffer(SharedImageBuffer & i_source_buffer)
    {
        m_source_buffer = i_source_buffer;
    }

    bool Pipeline::Run(std::shared_ptr<bool> & i_terminate, std::string i_output_key)
    {
        bool ret = true;

        if (!i_terminate)
        {
            std::cout << "Error: Input termination pointer to pipeline is uninitialised" << std::endl;
            return false;
        }

        // \todo create consumer task and producer task wrappers so pipeline can run without being a consumer

        // Read data from source buffer. If empty, wait
        while (!(*i_terminate && m_source_buffer->size() == 0))
        {
            // wait for source buffer to fill
            while (m_source_buffer->size() == 0)
            {
                if (*i_terminate)
                {
                    return true;
                }
            }

            // Read data from source buffer
            m_tray->SetVar(i_output_key, std::make_shared<cv::Mat>(m_source_buffer->front().clone()));
            m_source_buffer->pop_front();

            // Execute pipeline on source buffer contents
            for (const auto & pipeline : m_pipeline)
            {
                for (const auto & stage : pipeline)
                {
                    bool ret = stage->Run(m_tray);

                    if (ret == false)
                    {
                        break;
                    }
                }
            }
        }

        return ret;
    }
}