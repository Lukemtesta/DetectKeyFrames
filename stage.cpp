
#include "stage.h"

namespace pipeline
{
    Stage::Stage(std::string i_stage_name) :
        m_stage_name(i_stage_name)
    {
    };


    Stage::~Stage() {};


    bool Stage::Run(SharedTray & i_tray)
    {
        bool ret = false;

        try
        {
            ret = Process(i_tray);
        }
        catch (std::exception & exception)
        {
            std::cout << exception.what() << "\n" << std::endl;
            std::cout << "Error: Pipeline stage " + m_stage_name + " has exited prematurely. Stopping pipeline." << std::endl;
            throw;
        }

        return ret;
    }


    bool Stage::Process(SharedTray & i_tray)
    {
        i_tray;

        return true;
    }
}