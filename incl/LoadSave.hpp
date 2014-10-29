#ifndef INCL_LOADSAVE_HPP
#define INCL_LOADSAVE_HPP

#include <fstream>
#include <istream>
#include <ostream>
#include <stdexcept>

#include <NEATGenes.hpp>
#include <Constants.hpp>
#include <Enum.hpp>
#include <DirectorResults.hpp>
#include <Innovation.hpp>
#include <NEATGA.hpp>

static void saveNeuronsToFile(std::string fileName, std::vector<SNeuronGene> neurons)
{
    std::ofstream out(fileName.c_str());

    for(SNeuronGene& n : neurons)
    {
        n.NeuronTypeInt = static_cast<int>(n.NeuronType);

        out << n; // store the object to file
    }

    out.close();
}

static void saveLinksToFile(std::string fileName, std::vector<SLinkGene> links)
{
    std::ofstream out(fileName.c_str());

    for(SLinkGene& l : links)
    {
        out << l; // store the object to file
    }

    out.close();
}

static std::vector<SNeuronGene> loadNeuronsFromFile(std::string fileName)
{
    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::vector<SNeuronGene> neurons;

    SNeuronGene n;

    while(in >> n)
    {
        n.NeuronType = static_cast<neuron_type>(n.NeuronTypeInt);
        neurons.push_back(n);
    }

    in.close();

    return neurons;
}

static std::vector<SLinkGene> loadLinksFromFile(std::string fileName)
{
    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::vector<SLinkGene> links;

    SLinkGene l;

    while(in >> l)
    {
        links.push_back(l);
    }

    in.close();

    return links;
}

static void saveInnovationToFile(std::string fileName, std::vector<SInnovation> innovations)
{
    std::ofstream out(fileName.c_str());

    for(SInnovation& inno : innovations)
    {
        out << inno; // store the object to file
    }

    out.close();
}

static std::vector<SInnovation> loadInnovationFromFile(std::string fileName)
{
    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::vector<SInnovation> innovations;

    SInnovation inno;

    while(in >> inno)
    {
        inno.InnovationType = static_cast<innov_type>(inno.InnovationTypeInt);
        inno.NeuronType = static_cast<neuron_type>(inno.NeuronTypeInt);

        innovations.push_back(inno);
    }

    in.close();

    return innovations;
}

static void saveInnovationDataToFile(std::string fileName, int neuronID, int innoNum)
{
    std::ofstream out(fileName.c_str());

    out
    << neuronID
    << '\n'
    << innoNum;
//    << '\n'; // store the object to file

    out.close();
}

static std::pair<int, int> loadInnovationDataFromFile(std::string fileName)
{
    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::pair<int, int> params;

    in >> params.first;
    in >> params.second;

    in.close();

    return params;
}

static void saveSplitDepthsToFile(std::string fileName, std::vector<SplitDepth> splits)
{
    std::ofstream out(fileName.c_str());

    for(SplitDepth& split : splits)
    {
        out << split; // store the object to file
    }

    out.close();
}

static std::vector<SplitDepth> loadSplitDepthsFromFile(std::string fileName)
{
    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::vector<SplitDepth> splits;

    SplitDepth split;

    while(in >> split)
    {
        splits.push_back(split);
    }

    in.close();

    return splits;
}

static void saveSplitClientToFile(std::string fileName, std::pair<int, int> data)
{
    std::ofstream out(fileName.c_str());

    out
    << data.first
    << '\n'
    << data.second;

    out.close();
}

static std::pair<int, int> loadSplitClientFromFile(std::string fileName)
{
    std::ifstream in(fileName.c_str());

    std::pair<int, int> data;

    in
    >> data.first
    >> data.second;

    in.close();

    return data;
}

static void saveStats(std::string fileName, DirectorResults dR)
{
    std::ofstream out(fileName.c_str());

    out << "Total Time: " << dR.totalTime.asSeconds() << '\n';
    out << "Average Time: " << dR.avgTime.asSeconds() << '\n';
    out << "Level Time: " << dR.levelTime.asSeconds() << '\n';
    out << "Player Lives: " << dR.playerLives << '\n';
    out << "Player score: " << dR.playerScore << '\n';
    out << "Player Kills: " << dR.playerKills << '\n';
    out << "Kill percentage: " << dR.killPercent << '\n';
    out << "Harvester Num: " << dR.harvesterNum << '\n';
    out << "Hunter Num :" << dR.hunterNum << '\n';
    out << "Harvester Fitness: " << dR.harvesterBestFitness << '\n';
    out << "Hunter Fitness: " << dR.hunterBestFitness << '\n';
    out << "Enemy Score percentage: " << dR.enemyPercentageCollected << '\n';
    out << "Collectable ratio: " << dR.collectableRatio << '\n';
    out << "Mine Num: " << dR.mineNum << '\n';
    out << "Enemy mine hit: " << dR.mineHitNum << '\n';

//    out << dR; // store the object to file

    out.close();
}

static void saveHighScores(std::vector<std::string> names, std::vector<int> scores)
{
    std::string fileName("highscore/names.txt");

    std::ofstream out(fileName.c_str());

    for(std::string& name : names)
    {
        out << name << '\n';
    }

    out.close();

    fileName.clear();
    fileName = "highscore/scores.dat";

    out.open(fileName.c_str());

    for(int& score : scores)
    {
        out << score << '\n';
    }
}

static std::vector<std::string> loadHighScoreNamesFromFile()
{
    std::string fileName("highscore/names.txt");

    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::vector<std::string> names;

    std::string str;

    while(in >> str)
    {
        names.push_back(str);
    }

    in.close();

    return names;
}

static std::vector<int> loadHighScoresFromFile()
{
    std::string fileName("highscore/scores.dat");
    std::ifstream in(fileName.c_str());

    if(!in.is_open())
        throw std::runtime_error("Error loading file " + fileName);

    std::vector<int> scores;

    int n;

    while(in >> n)
    {
        scores.push_back(n);
    }

    in.close();

    return scores;
}

static void saveGenerationStatsToFile(std::string fileName, std::vector<double> stats)
{
    std::ofstream out(fileName, std::fstream::app);

    out
    << stats[0] << ' '
    << stats[1] << ' '
    << stats[2] << ' '
    << stats[3] << ' '
    << stats[4] << ' '
    << stats[5] << '\n';

    out.close();
}

//static void savePopParams(std::string fileName, std::vector<int> params)
//{
//    std::ofstream out(fileName);
//
//    out
//    << params[0]
//}
//
//
//const int iNumBestSweepers = 20;
//const int NumTicksPerGen  = 2000;
//const int NumGen = 25;
//const int NumPop = 30;
//const int SpawnPool = 50;


#endif //INCL_LOADSAVE_HPP
