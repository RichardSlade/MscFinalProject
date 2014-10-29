#ifndef NEATGENOTYPE_HPP
#define NEATGENOTYPE_HPP
//-----------------------------------------------------------------------
//
//  Name: genotype.h
//
//  Author: Mat Buckland 2002
//
//	Desc: genome definition used in the implementation of
//        Kenneth Owen Stanley's and Risto Miikkulainen's NEAT idea.
//
//-----------------------------------------------------------------------
#include <vector>

#include <NEATPhenotype.hpp>
#include <Utility.hpp>
#include <Innovation.hpp>
#include <NEATGenes.hpp>

using namespace std;


class NEATGA;
class Innovation;


//------------------------------------------------------------------------
//
// NEATGenotype class definition. A genome basically consists of a vector of
// link genes, a vector of neuron genes and a fitness score.
//------------------------------------------------------------------------
class NEATGenotype
{

private:

  //its identification number
  int                     m_GenomeID;
  int                       mID;
    int                  mLifeTicks;

    bool                mActive;

  //all the neurons which make up this genome
  vector<SNeuronGene>     m_vecNeurons;

  //and all the the links
  vector<SLinkGene>       m_vecLinks;

  //pointer to its phenotype
  NEATNeuralNet*             m_pPhenotype;

  //its raw fitness score
  double                  m_dFitness;

  //its fitness score after it has been placed into a
  //species and adjusted accordingly
  double                  m_dAdjustedFitness;

  //the number of offspring this individual is required to spawn
  //for the next generation
  double                  m_dAmountToSpawn;

  //keep a record of the number of inputs and outputs
  int                     m_iNumInputs,
                          m_iNumOutPuts;

  //keeps a track of which species this genome is in (only used
  //for display purposes)
  int                     m_iSpecies;

  //returns true if the specified link is already part of the genome
  bool    DuplicateLink(int NeuronIn, int NeuronOut);

  //given a neuron id this function just finds its position in
  //m_vecNeurons
  int     GetElementPos(int neuron_id);

  //tests if the passed ID is the same as any existing neuron IDs. Used
  //in AddNeuron
  bool    AlreadyHaveThisNeuronID(const int ID);


public:

  NEATGenotype();

  //this constructor creates a minimal genome where there are output &
  //input neurons and every input neuron is connected to each output neuron
  NEATGenotype(int id, int inputs, int outputs);

  NEATGenotype(int id, int id2, int inputs, int outputs);

  //this constructor creates a genome from a vector of SLinkGenes
  //a vector of SNeuronGenes and an ID number
  NEATGenotype(int                 id,
          vector<SNeuronGene> neurons,
          vector<SLinkGene>   genes,
          int                 inputs,
          int                 outputs);

  ~NEATGenotype();

  //copy constructor
  NEATGenotype(const NEATGenotype& g);

  //assignment operator
  NEATGenotype& operator =(const NEATGenotype& g);

  //create a neural network from the genome
  NEATNeuralNet*	        CreatePhenotype(int depth);

  //delete the neural network
  void                DeletePhenotype();

  //add a link to the genome dependent upon the mutation rate
  void                AddLink(double      MutationRate,
                              double      ChanceOfRecurrent,
                              Innovation &innovation,
                              int         NumTrysToFindLoop,
                              int         NumTrysToAddLink);

  //and a neuron
  void                AddNeuron(double      MutationRate,
                                Innovation &innovation,
                                int         NumTrysToFindOldLink);

  //this function mutates the connection weights
  void                MutateWeights(double  mut_rate,
                                    double  prob_new_mut,
                                    double  dMaxPertubation);

  //perturbs the activation responses of the neurons
  void                MutateActivationResponse(double mut_rate,
                                               double MaxPertubation);

  //calculates the compatibility score between this genome and
  //another genome
  double              GetCompatibilityScore(const NEATGenotype &genome);

  void                SortGenes();

  //overload '<' used for sorting. From fittest to poorest.
  friend bool operator<(const NEATGenotype& lhs, const NEATGenotype& rhs)
  {
    return (lhs.m_dFitness > rhs.m_dFitness);
  }


	//---------------------------------accessor methods
	int	    ID()const{return m_GenomeID;}
	int     getID(){return mID;}
	void    setID(int val){ mID = val; }
  void    SetID(const int val){m_GenomeID = val;}

	int     NumGenes()const{return m_vecLinks.size();}
  int     NumNeurons()const{return m_vecNeurons.size();}
  int     NumInputs()const{return m_iNumInputs;}
  int     NumOutputs()const{return m_iNumOutPuts;}

  double  AmountToSpawn()const{return m_dAmountToSpawn;}
  void    SetAmountToSpawn(double num){m_dAmountToSpawn = num;}

  void    SetFitness(const double num){m_dFitness = num;}
  void    SetAdjFitness(const double num){m_dAdjustedFitness = num;}
  double  Fitness()const{return m_dFitness;}
  double  GetAdjFitness()const{return m_dAdjustedFitness;}

  int     GetSpecies()const{return m_iSpecies;}
  void    SetSpecies(int spc){m_iSpecies = spc;}

  double  SplitY(const int val)const{return m_vecNeurons[val].dSplitY;}

  vector<SLinkGene>	  Genes()const{return m_vecLinks;}
  vector<SNeuronGene> Neurons()const{return m_vecNeurons;}

  vector<SLinkGene>::iterator StartOfGenes(){return m_vecLinks.begin();}
  vector<SLinkGene>::iterator EndOfGenes(){return m_vecLinks.end();}

    void  incLifeTick(){ mLifeTicks ++; }
  int   getLifeTick(){ return mLifeTicks; }
  void  resetLifeTick(){ mLifeTicks = 0; }
  void  setActive(bool status){mActive = status;}
  bool  isActive(){return mActive;}
};



#endif
