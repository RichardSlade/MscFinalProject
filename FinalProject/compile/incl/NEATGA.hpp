#ifndef NEATGA_HPP
#define	NEATGA_HPP

//------------------------------------------------------------------------
//
//	Name: NEATGA.h
//
//  Author: Mat Buckland 2002
//
//  Desc: The evolutionary algorithm class  used in the implementation of
//        Kenneth Owen Stanley's and Risto Miikkulainen's NEAT idea.
//
//------------------------------------------------------------------------

#include <vector>
//#include <windows.h>

#include <NEATPhenotype.hpp>
#include <NEATGenotype.hpp>
#include <Species.hpp>
#include <Constants.hpp>

using namespace std;



//------------------------------------------------------------------------
//
//  this structure is used in the creation of a network depth lookup
//  table.
//------------------------------------------------------------------------
struct SplitDepth
{
  double val;

  int    depth;

  SplitDepth(double v, int d):val(v), depth(d){}

  SplitDepth(){};

  // Insertion operator
    friend std::ostream& operator<<(std::ostream& os, const SplitDepth& sD)
    {
        // write out individual members of s with an end of line between each one
        os << sD.val << '\n';
        os << sD.depth << '\n';

        return os;
    }

    // Extraction operator
    friend std::istream& operator>>(std::istream& is, SplitDepth& sD)
    {
        // read in individual members of s
        is
        >> sD.val
        >> sD.depth;

        return is;
    }

};


//------------------------------------------------------------------------
//
//------------------------------------------------------------------------
class NEATGA
{

    public:
        typedef std::unique_ptr<NEATGA> GAptr;

private:

	//current population
	vector<NEATGenotype>   m_vecGenomes;

	//keep a record of the last generations best genomes. (used to render
  //the best performers to the display if the user presses the 'B' key)
	vector<NEATGenotype>   m_vecBestGenomes;

	//all the species
  vector<Species>  m_vecSpecies;

	//to keep track of innovations
  Innovation*      m_pInnovation;

	//current generation
  int               m_iGeneration;

  int               mWorstAdjIndvIndex;

	int               m_iNextGenomeID;
	int                mNextID;

  int               m_iNextSpeciesID;

	int               m_iPopSize;

    int             mNumIn;
    int             mNumOut;

  //adjusted fitness scores
  double            m_dTotFitAdj,
                    m_dAvFitAdj;

  //index into the genomes for the fittest genome
  int               m_iFittestGenome;

  double            m_dBestEverFitness;

  //local copy of app handle
 // HWND              m_hwnd;

  //local copies of client area
  int               cxClient, cyClient;

  //this holds the precalculated split depths. They are used
  //to calculate a neurons x/y position for rendering and also
  //for calculating the flush depth of the network when a
  //phenotype is working in 'snapshot' mode.
  vector<SplitDepth> vecSplits;


	//used in Crossover
	void    AddNeuronID(int nodeID, vector<int> &vec);

  //this function resets some values ready for the next epoch, kills off
  //all the phenotypes and any poorly performing species.
  void    ResetAndKill(bool isRealTime);

  //separates each individual into its respective species by calculating
  //a compatibility score with every other member of the population and
  //niching accordingly. The function then adjusts the fitness scores of
  //each individual by species age and by sharing and also determines
  //how many offspring each individual should spawn.
  void    SpeciateAndCalculateSpawnLevels(bool isRealTime);

  //adjusts the fitness scores depending on the number sharing the
  //species and the age of the species.
  void    AdjustSpeciesFitnesses();

  NEATGenotype Crossover(NEATGenotype& mum, NEATGenotype& dad);

  NEATGenotype TournamentSelection(const int NumComparisons);

  //searches the lookup table for the dSplitY value of each node in the
  //genome and returns the depth of the network based on this figure
  int     CalculateNetDepth(const NEATGenotype &gen);

  //sorts the population into descending fitness, keeps a record of the
  //best n genomes and updates any fitness statistics accordingly
  void    SortAndRecord();

  //a recursive function used to calculate a lookup table of split
  //depths
  vector<SplitDepth> Split(double low, double high, int depth);


public:

	NEATGA(int		size,
              int		inputs,
              int		outputs,
//              HWND  hwnd,
              int   cx,
              int   cy);

	~NEATGA();

	vector<NEATNeuralNet*>	 Epoch(const vector<double> &FitnessScores);
	vector<NEATNeuralNet*>	 gameEpoch(const vector<double> &FitnessScores);



	pair<pair<int, int>, NEATNeuralNet*> rtEpoch(const vector<double> &FitnessScores);

	//iterates through the population and creates the phenotypes
  vector<NEATNeuralNet*>  CreatePhenotypes(int num);

  //keeps a record of the n best genomes from the last population.
  //(used to display the best genomes)
  void                 StoreBestGenomes();
  void                  archiveGenomes();

  void                  resetGA();

  //renders the best performing species statistics and a visual aid
  //showing the distribution.
//  void                 RenderSpeciesInfo(HDC &surface, RECT db);

  //returns a vector of the n best phenotypes from the previous generation
  vector<NEATNeuralNet*>  GetBestPhenotypesFromLastGeneration();


  //----------------------------------------------------accessor methods
  int     NumSpecies()const{return m_vecSpecies.size();}

    void    createGenotypes();
  void      setGenotypes(std::vector<NEATGenotype> genotypes){ m_vecGenomes.clear(); m_vecGenomes = genotypes; }

  std::vector<NEATGenotype>     getBestGenotypes(){ return m_vecBestGenomes; }

  double  BestEverFitness()const{return m_dBestEverFitness;}

    void setInnovation(Innovation inno);

    void setSplitDepths(std::vector<SplitDepth> splits){ vecSplits = splits; }
    void setSplitClient(std::pair<int, int> client){ cxClient = client.first; cyClient = client.second;}

    int                                 getNeuronID(){ return m_pInnovation->getNeuronID(); }
    int                                 getInnoNum(){ return m_pInnovation->getInnoNum(); }
    std::vector<SInnovation>&           getInnovations(){ return m_pInnovation->getInnovations(); }
    std::vector<SplitDepth>&            getSplitDepths(){ return vecSplits; }
    std::pair<int, int>                getSplitClient(){ std::pair<int, int>(cxClient, cyClient); }
    void                                setPopSize(int val){ m_iPopSize = val; }

    void                                tickGA();
//    double                              getAvgFitness(){ return mAvgFitness; }
    void                                resetTimes();

};


#endif
