/////////////////////////////////////////////////////////////////////////////
/// Author:      Ahmed Hamdi Boujelben <ahmed.hamdi.boujelben@gmail.com>
/// Created:     2018
/// Copyright:   (c) 2018 Ahmed Hamdi Boujelben
/// Licence:     Attribution-NonCommercial 4.0 International
/////////////////////////////////////////////////////////////////////////////

#include "renderer.h"
#include "network/iterator.h"
#include "misc/userInput.h"
#include "misc/tools.h"
#include "misc/maths.h"

#include <libs/csvParser/csv.h>

#include <QDir>

#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace PNM
{

void renderer::run()
{
    loadStateFiles();
    processFrames();
}

std::string renderer::getNotification()
{
    std::ostringstream ss;
    ss << "Post-processing Network State Files \n"
       << std::fixed << std::setprecision(2)
       << currentFileIndex << " out of " << totalFiles;
    return ss.str();
}

int renderer::getProgress()
{
    return currentFileIndex * 100 / totalFiles;
}

void renderer::loadStateFiles()
{
    QDir directory(userInput::get().pathToNetworkStateFiles.c_str());
    QStringList stateFiles = directory.entryList(QStringList() << "network_state*.nums", QDir::Files);

    totalFiles = stateFiles.length();

    for (QString filename : stateFiles)
    {

        int phaseFlag(0);
        double concentration(0);
        io::CSVReader<2> in(userInput::get().pathToNetworkStateFiles + "/" + filename.toStdString());
        in.read_header(io::ignore_missing_column, "phase", "concentration");

        for (element *e : pnmRange<element>(network))
        {
            if (in.read_row(phaseFlag, concentration))
            {
                e->setPhaseFlag(static_cast<phase>(phaseFlag));
                e->setConcentration(concentration);
            }
        }

        currentFileIndex++;
        emit notifyGUI();

        if (simulationInterrupted)
            break;
    }
}

void renderer::processFrames()
{
    tools::renderVideo(PNM::userInput::get().rendererFPS);
    if (!PNM::userInput::get().keepFrames)
        tools::cleanVideosFolder();
}

} // namespace PNM
