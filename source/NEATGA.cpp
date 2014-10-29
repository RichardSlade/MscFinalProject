#include <NEATGA.hpp>
#include <Constants.hpp>

//------------------------------------------------------------------------
//
//	Name: NEATGA.cpp
//
//  Author: Mat Buckland 2002
//
//------------------------------------------------------------------------

//-------------------------------------------------------------------------
//	this constructor creates a base genome from supplied values and creates
//	a population of 'size' similar (same topology, varying weights) genomes
//-------------------------------------------------------------------------
NEATGA::NEATGA(int  size,
         int  inputs,
         int  outputs,
         //HWND hwnd,
         int  cx,
         int  cy):  m_iPopSize(size),
                    m_iGeneration(0),
                    m_pInnovation(NULL),
                    m_iNextGenomeID(0),
                    mNextID(0),
                    m_iNextSpeciesID(0),
                    m_iFittestGenome(0),
                    m_dBestEverFitness(0),
                    m_dTotFitAdj(0),
                    m_dAvFitAdj(0),
                    //m_hwnd(hwnd),
                    cxClient(cx),
                    cyClient(cy)
{
    mNumIn = inputs;
    mNumOut = outputs;

	//create the innovation list. First create a minimal genome
	NEATGenotype genome(1, inputs, outputs);

	//create the innovations
  m_pInnovation = new Innovation(genome.Genes(), genome.Neurons());

  //create the network depth lookup table
  vecSplits = Split(0, 1, 0);
}

void NEATGA::resetTimes()
{
    for(NEATGenotype& g : m_vecGenomes)
        g.resetLifeTick();
}

void NEATGA::tickGA()
{
    for(NEATGenotype& g : m_vecGenomes)
    {
        g.incLifeTick();
    }
}

void NEATGA::setInnovation(Innovation inno)
{
    m_pInnovation->setInnovations(inno.getInnovations());
    m_pInnovation->setNeuronID(inno.getNeuronID());
    m_pInnovation->setInnoNum(inno.getInnoNum());
}

//------------------------------------- dtor -----------------------------
//
//------------------------------------------------------------------------
NEATGA::~NEATGA()
{
    m_vecBestGenomes.clear();
    m_vecGenomes.clear();

  if(m_pInnovation)
  {
    delete m_pInnovation;

    m_pInnovation = NULL;
  }
}

// Resets the whole GA ready for new generation
void NEATGA::resetGA()
{
    m_vecGenomes.clear();
//    m_vecBestGenomes.clear();

    m_dBestEverFitness = 0.0;

    //create the population of genomes
	for (int i=0; i<m_iPopSize; ++i)
	{
        m_vecGenomes.push_back(NEATGenotype(m_iNextGenomeID++, mNumIn, mNumOut));
	}
}

void NEATGA::createGenotypes()
{
    //create the population of genomes
	for (int i=0; i<m_iPopSize; ++i)
	{
        m_vecGenomes.push_back(NEATGenotype(m_iNextGenomeID++, mNumIn, mNumOut));
	}

	for(NEATGenotype& g : m_vecGenomes)
        g.setID(mNextID++);
}

//-------------------------------CreatePhenotypes-------------------------
//
//	cycles through all the members of the population and creates their
//  phenotypes. Returns a vector containing pointers to the new phenotypes
//-------------------------------------------------------------------------
vector<NEATNeuralNet*> NEATGA::CreatePhenotypes(int num)
{
	vector<NEATNeuralNet*> networks;

    int numPhenos = m_iPopSize;

    if(num != -1)
        numPhenos = num;

  for (int i=0; i< numPhenos; i++)
	{
    //calculate max network depth
    int depth = CalculateNetDepth(m_vecGenomes[i]);

		//create new phenotype
		NEATNeuralNet* net = m_vecGenomes[i].CreatePhenotype(depth);

    networks.push_back(net);
	}

	return networks;
}

//-------------------------- CalculateNetDepth ---------------------------
//
//  searches the lookup table for the dSplitY value of each node in the
//  genome and returns the depth of the network based on this figure
//------------------------------------------------------------------------
int NEATGA::CalculateNetDepth(const NEATGenotype &gen)
{
  int MaxSoFar = 0;

  for (int nd=0; nd<gen.NumNeurons(); ++nd)
  {
    for (int i=0; i<vecSplits.size(); ++i)
    {

      if ((gen.SplitY(nd) == vecSplits[i].val) &&
          (vecSplits[i].depth > MaxSoFar))
      {
        MaxSoFar = vecSplits[i].depth;
      }
    }
  }
  return MaxSoFar + 2;
}


//-----------------------------------AddNeuronID----------------------------
//
//	just checks to see if a node ID has already been added to a vector of
//  nodes. If not 	then the new ID  gets added. Used in Crossover.
//------------------------------------------------------------------------
void NEATGA::AddNeuronID(const int nodeID, vector<int> &vec)
{
	for (int i=0; i<vec.size(); i++)
	{
		if (vec[i] == nodeID)
		{
			//already added
			return;
		}
	}

	vec.push_back(nodeID);

	return;
}

//------------------------------------- Epoch ----------------------------
//
//  This function performs one epoch of the genetic algorithm and returns
//  a vector of pointers to the new phenotypes
//------------------------------------------------------------------------
vector<NEATNeuralNet*> NEATGA::Epoch(const vector<double> &FitnessScores)
{
  //first check to make sure we have the correct amount of fitness scores
  if (FitnessScores.size() != m_vecGenomes.size())
  {
//    MessageBox(NULL,"NEATGA::Epoch(scores/ genomes mismatch)!","Error", MB_OK);
  }

  //reset appropriate values and kill off the existing phenotypes and
  //any poorly performing species
  ResetAndKill(false);

  //update the genomes with the fitnesses scored in the last run
  for (int gen=0; gen<m_vecGenomes.size(); ++gen)
  {
    m_vecGenomes[gen].SetFitness(FitnessScores[gen]);
  }

    sort(m_vecGenomes.begin(), m_vecGenomes.end());

  //is the best genome this generation the best ever?
  if (m_vecGenomes[0].Fitness() > m_dBestEverFitness)
  {
    m_dBestEverFitness = m_vecGenomes[0].Fitness();
  }

  archiveGenomes();

  //separate the population into species of similar topology, adjust
  //fitnesses and calculate spawn levels
  SpeciateAndCalculateSpawnLevels(false);

  //this will hold the new population of genomes
  vector<NEATGenotype> NewPop;

  //request the offspring from each species. The number of children to
  //spawn is a double which we need to convert to an int.
  int NumSpawnedSoFar = static_cast<int>(iNumBestSweepers / 4);

  NEATGenotype baby;

  for(int i = 0; i < static_cast<int>(iNumSweepers / 4); i++)
    NewPop.push_back(m_vecBestGenomes[i]);

    if(m_iGeneration == 0)
        m_dBestEverFitness = m_vecGenomes[0].Fitness();

  //now to iterate through each species selecting offspring to be mated and
  //mutated
  for (int spc=0; spc<m_vecSpecies.size(); ++spc)
  {
    //because of the number to spawn from each species is a double
    //rounded up or down to an integer it is possible to get an overflow
    //of genomes spawned. This statement just makes sure that doesn't
    //happen
    if (NumSpawnedSoFar < iNumSweepers)
    {
      //this is the amount of offspring this species is required to
      // spawn. Rounded simply rounds the double up or down.
      int NumToSpawn = Rounded(m_vecSpecies[spc].NumToSpawn());

      bool bChosenBestYet = false;

      while (NumToSpawn--)
      {
        //first grab the best performing genome from this species and transfer
        //to the new population without mutation. This provides per species
        //elitism
        if (!bChosenBestYet)
        {
          baby = m_vecSpecies[spc].Leader();

          bChosenBestYet = true;
        }

        else
        {
          //if the number of individuals in this species is only one
          //then we can only perform mutation
          if (m_vecSpecies[spc].NumMembers() == 1)
          {
            //spawn a child
            baby = m_vecSpecies[spc].Spawn();
          }

          //if greater than one we can use the crossover operator
          else
          {
            //spawn1
            NEATGenotype g1 = m_vecSpecies[spc].Spawn();

            if (randFloat() < dCrossoverRate)
            {

              //spawn2, make sure it's not the same as g1
              NEATGenotype g2 = m_vecSpecies[spc].Spawn();

              //number of attempts at finding a different genome
              int NumAttempts = 5;

              while ( (g1.ID() == g2.ID()) && (NumAttempts--) )
              {
                g2 = m_vecSpecies[spc].Spawn();
              }

              if (g1.ID() != g2.ID())
              {
                baby = Crossover(g1, g2);
              }
            }

            else
            {
              baby = g1;
            }
          }


          ++m_iNextGenomeID;

          baby.SetID(m_iNextGenomeID);

          //now we have a spawned child lets mutate it! First there is the
          //chance a neuron may be added
          if (baby.NumNeurons() < iMaxPermittedNeurons)
          {
            baby.AddNeuron(dChanceAddNode,
                           *m_pInnovation,
                           iNumTrysToFindOldLink);
          }

          //now there's the chance a link may be added
          baby.AddLink(dChanceAddLink,
                       dChanceAddRecurrentLink,
                       *m_pInnovation,
                       iNumTrysToFindLoopedLink,
                       iNumAddLinkAttempts);

          //mutate the weights
          baby.MutateWeights(dMutationRate,
                             dProbabilityWeightReplaced,
                             dMaxWeightPerturbation);

          baby.MutateActivationResponse(dActivationMutationRate,
                                        dMaxActivationPerturbation);
        }

        //sort the babies genes by their innovation numbers
        baby.SortGenes();

        //add to new pop
        NewPop.push_back(baby);

        ++NumSpawnedSoFar;

        if (NumSpawnedSoFar == iNumSweepers)
        {
          NumToSpawn = 0;
        }

      }//end while

    }//end if

  }//next species


  //if there is an underflow due to the rounding error and the amount
  //of offspring falls short of the population size additional children
  //need to be created and added to the new population. This is achieved
  //simply, by using tournament selection over the entire population.
  if (NumSpawnedSoFar < iNumSweepers)
  {

    //calculate amount of additional children required
    int Rqd = iNumSweepers - NumSpawnedSoFar;

    //grab them
    while (Rqd--)
    {
      NewPop.push_back(TournamentSelection(m_iPopSize/5));
    }
  }

  //replace the current population with the new one
  m_vecGenomes = NewPop;

  //create the new phenotypes
  vector<NEATNeuralNet*> new_phenotypes;

  for (int gen=0; gen<m_vecGenomes.size(); ++gen)
  {
    //calculate max network depth
    int depth = CalculateNetDepth(m_vecGenomes[gen]);

    NEATNeuralNet* phenotype = m_vecGenomes[gen].CreatePhenotype(depth);

    new_phenotypes.push_back(phenotype);
  }

  //increase generation counter
  ++m_iGeneration;

  return new_phenotypes;
}


vector<NEATNeuralNet*> NEATGA::gameEpoch(const vector<double> &FitnessScores)
{
  //first check to make sure we have the correct amount of fitness scores
  if (FitnessScores.size() != m_vecGenomes.size())
  {
//    MessageBox(NULL,"NEATGA::Epoch(scores/ genomes mismatch)!","Error", MB_OK);
  }

  //reset appropriate values and kill off the existing phenotypes and
  //any poorly performing species
  ResetAndKill(false);

  //update the genomes with the fitnesses scored in the last run
  for (int gen=0; gen<m_vecGenomes.size(); ++gen)
  {
    m_vecGenomes[gen].SetFitness(FitnessScores[gen]);
  }

    sort(m_vecGenomes.begin(), m_vecGenomes.end());

  //separate the population into species of similar topology, adjust
  //fitnesses and calculate spawn levels
  SpeciateAndCalculateSpawnLevels(false);

  //this will hold the new population of genomes
  vector<NEATGenotype> NewPop;

  //request the offspring from each species. The number of children to
  //spawn is a double which we need to convert to an int.
  int NumSpawnedSoFar = static_cast<int>(iNumBestSweepers / 4);

  NEATGenotype baby;

  //now to iterate through each species selecting offspring to be mated and
  //mutated
  for (int spc=0; spc<m_vecSpecies.size(); ++spc)
  {
    //because of the number to spawn from each species is a double
    //rounded up or down to an integer it is possible to get an overflow
    //of genomes spawned. This statement just makes sure that doesn't
    //happen
    if (NumSpawnedSoFar < iNumSweepers)
    {
      //this is the amount of offspring this species is required to
      // spawn. Rounded simply rounds the double up or down.
      int NumToSpawn = Rounded(m_vecSpecies[spc].NumToSpawn());

      bool bChosenBestYet = false;

      while (NumToSpawn--)
      {
        //first grab the best performing genome from this species and transfer
        //to the new population without mutation. This provides per species
        //elitism
        if (!bChosenBestYet)
        {
          baby = m_vecSpecies[spc].Leader();

          bChosenBestYet = true;
        }

        else
        {
          //if the number of individuals in this species is only one
          //then we can only perform mutation
          if (m_vecSpecies[spc].NumMembers() == 1)
          {
            //spawn a child
            baby = m_vecSpecies[spc].Spawn();
          }

          //if greater than one we can use the crossover operator
          else
          {
            //spawn1
            NEATGenotype g1 = m_vecSpecies[spc].Spawn();

            if (randFloat() < dCrossoverRate)
            {

              //spawn2, make sure it's not the same as g1
              NEATGenotype g2 = m_vecSpecies[spc].Spawn();

              //number of attempts at finding a different genome
              int NumAttempts = 5;

              while ( (g1.ID() == g2.ID()) && (NumAttempts--) )
              {
                g2 = m_vecSpecies[spc].Spawn();
              }

              if (g1.ID() != g2.ID())
              {
                baby = Crossover(g1, g2);
              }
            }

            else
            {
              baby = g1;
            }
          }


          ++m_iNextGenomeID;

          baby.SetID(m_iNextGenomeID);

          //now we have a spawned child lets mutate it! First there is the
          //chance a neuron may be added
          if (baby.NumNeurons() < iMaxPermittedNeurons)
          {
            baby.AddNeuron(dChanceAddNode,
                           *m_pInnovation,
                           iNumTrysToFindOldLink);
          }

          //now there's the chance a link may be added
          baby.AddLink(dChanceAddLink,
                       dChanceAddRecurrentLink,
                       *m_pInnovation,
                       iNumTrysToFindLoopedLink,
                       iNumAddLinkAttempts);

          //mutate the weights
          baby.MutateWeights(dMutationRate,
                             dProbabilityWeightReplaced,
                             dMaxWeightPerturbation);

          baby.MutateActivationResponse(dActivationMutationRate,
                                        dMaxActivationPerturbation);
        }

        //sort the babies genes by their innovation numbers
        baby.SortGenes();

        //add to new pop
        NewPop.push_back(baby);

        ++NumSpawnedSoFar;

        if (NumSpawnedSoFar == iNumSweepers)
        {
          NumToSpawn = 0;
        }

      }//end while

    }//end if

  }//next species


  //if there is an underflow due to the rounding error and the amount
  //of offspring falls short of the population size additional children
  //need to be created and added to the new population. This is achieved
  //simply, by using tournament selection over the entire population.
  if (NumSpawnedSoFar < iNumSweepers)
  {

    //calculate amount of additional children required
    int Rqd = iNumSweepers - NumSpawnedSoFar;

    //grab them
    while (Rqd--)
    {
      NewPop.push_back(TournamentSelection(m_iPopSize/5));
    }
  }

  //replace the current population with the new one
  m_vecGenomes = NewPop;

  //create the new phenotypes
  vector<NEATNeuralNet*> new_phenotypes;

  for (int gen=0; gen<m_vecGenomes.size(); ++gen)
  {
    //calculate max network depth
    int depth = CalculateNetDepth(m_vecGenomes[gen]);

    NEATNeuralNet* phenotype = m_vecGenomes[gen].CreatePhenotype(depth);

    new_phenotypes.push_back(phenotype);
  }

  //increase generation counter
  ++m_iGeneration;

  return new_phenotypes;
}


std::pair<std::pair<int, int>, NEATNeuralNet*> NEATGA::rtEpoch(const vector<double> &FitnessScores)
{
     //first check to make sure we have the correct amount of fitness scores
    if (FitnessScores.size() != m_vecGenomes.size())
    {
        throw std::runtime_error("NEATGA::Epoch(scores/ genomes mismatch)!" + std::to_string(FitnessScores.size()) + " | " + 		std::to_string(m_vecGenomes.size()));
    }

    //reset appropriate values and kill off the existing phenotypes and
    //any poorly performing species
    ResetAndKill(true);

    //update the genomes with the fitnesses scored in the last run
    for(int gen = 0; gen < m_vecGenomes.size(); ++gen)
    {
        m_vecGenomes[gen].SetFitness(FitnessScores[gen]);
    }

    SpeciateAndCalculateSpawnLevels(false);

    m_vecGenomes[mWorstAdjIndvIndex].setActive(false);

    SpeciateAndCalculateSpawnLevels(true);

    NEATGenotype baby;

    if(m_vecSpecies.size() == 0)
        throw std::runtime_error("No species exist");

    double sumSpeciesFitness = 0.0;

    // Calculate total average fitness
    for(Species& sp : m_vecSpecies)
    {
        sumSpeciesFitness += sp.calculateAvgFitness();
    }

    int parentSpeciesIndex = -1;
    bool found = false;
    int findSpeciesAttempts = 50;

    if(m_vecSpecies.size() > 1)
    {
        while(findSpeciesAttempts --)
        {
            float r = randFloat();
            int randSpeciesIndex = rangedInt(0, m_vecSpecies.size() -1);

            if(r < (m_vecSpecies[randSpeciesIndex].avgFitness() / sumSpeciesFitness))
            {
                parentSpeciesIndex = randSpeciesIndex;
                found = true;
                break;
            }
        }
    }
    else
    {
        parentSpeciesIndex = 0;
        found = true;
    }

    if(!found)
    {
        parentSpeciesIndex = rangedInt(0, m_vecSpecies.size());
    }

    if(parentSpeciesIndex == -1)
        throw std::runtime_error("No species");

//    if the number of individuals in this species is only one
//    then we can only perform mutation
    if (m_vecSpecies[parentSpeciesIndex].NumMembers() == 1)
    {
        //spawn a child
        baby = m_vecSpecies[parentSpeciesIndex].Spawn();
    }
//    if greater than one we can use the crossover operator
    else
    {
        NEATGenotype g1 = m_vecSpecies[parentSpeciesIndex].Spawn();

        if (randFloat() < dCrossoverRate)
        {
            //spawn2, make sure it's not the same as g1
            NEATGenotype g2 = m_vecSpecies[parentSpeciesIndex].Spawn();

            //number of attempts at finding a different genome
            int NumAttempts = 5;

            while ( (g1.ID() == g2.ID()) && (NumAttempts--) )
            {
                g2 = m_vecSpecies[parentSpeciesIndex].Spawn();
            }

            if (g1.ID() != g2.ID())
            {
                baby = Crossover(g1, g2);
                if(baby.NumNeurons() == 0)
                    throw std::runtime_error("GA no neurons first");
            }
            else
            {
                baby = g2;
                if(baby.NumNeurons() == 0)
                    throw std::runtime_error("GA no neurons second");
            }
        }
        else
        {
            baby = g1;
            if(baby.NumNeurons() == 0)
                    throw std::runtime_error("GA no neurons third");
        }
    }

    baby.SetID(m_iNextGenomeID);
    baby.setID(mNextID++);

    m_iNextGenomeID ++;

    //now we have a spawned child lets mutate it! First there is the
    //chance a neuron may be added
    if (baby.NumNeurons() < iMaxPermittedNeurons)
    {
        baby.AddNeuron(dChanceAddNode,
                   *m_pInnovation,
                   iNumTrysToFindOldLink);
    }

    //now there's the chance a link may be added
    baby.AddLink(dChanceAddLink,
               dChanceAddRecurrentLink,
               *m_pInnovation,
               iNumTrysToFindLoopedLink,
               iNumAddLinkAttempts);

    //mutate the weights
    baby.MutateWeights(dMutationRate,
                     dProbabilityWeightReplaced,
                     dMaxWeightPerturbation);

    baby.MutateActivationResponse(dActivationMutationRate,
                                dMaxActivationPerturbation);

    //sort the babies genes by their innovation numbers
    baby.SortGenes();

    if(baby.NumNeurons() == 0)
        throw std::runtime_error("GA no neurons");

    int enemyIndex = mWorstAdjIndvIndex;
    int enemyID = m_vecGenomes[mWorstAdjIndvIndex].getID();

    m_vecGenomes[enemyIndex] = baby;
    //replace the current population with the new one

    //calculate max network depth
    int depth = CalculateNetDepth(m_vecGenomes[enemyIndex]);
    NEATNeuralNet* phenotype = m_vecGenomes[enemyIndex].CreatePhenotype(depth);
    std::pair<std::pair<int, int>, NEATNeuralNet*> newEnemyData;

    newEnemyData.first.first = enemyID;
    newEnemyData.first.second = m_vecGenomes[enemyIndex].getID();
    newEnemyData.second = phenotype;

    return newEnemyData;
}

//--------------------------- SortAndRecord-------------------------------
//
//  sorts the population into descending fitness, keeps a record of the
//  best n genomes and updates any fitness statistics accordingly
//------------------------------------------------------------------------
void NEATGA::SortAndRecord()
{
  //sort the genomes according to their unadjusted (no fitness sharing)
  //fitnesses
  sort(m_vecGenomes.begin(), m_vecGenomes.end());

  //is the best genome this generation the best ever?
  if (m_vecGenomes[0].Fitness() > m_dBestEverFitness)
  {
    m_dBestEverFitness = m_vecGenomes[0].Fitness();
  }

  //keep a record of the n best genomes
  StoreBestGenomes();
}

//----------------------------- StoreBestGenomes -------------------------
//
//  used to keep a record of the previous populations best genomes so that
//  they can be displayed if required.
//------------------------------------------------------------------------
void NEATGA::StoreBestGenomes()
{
  //clear old record
  m_vecBestGenomes.clear();

  for (int gen=0; gen<iNumBestSweepers; ++gen)
  {
    m_vecBestGenomes.push_back(m_vecGenomes[gen]);
  }
}

void NEATGA::archiveGenomes()
{
    std::vector<NEATGenotype> tempVec = m_vecBestGenomes;
    m_vecBestGenomes.clear();

    for (int gen=0; gen<iNumBestSweepers; ++gen)
    {
        tempVec.push_back(m_vecGenomes[gen]);
    }

    std::sort(tempVec.begin(), tempVec.end());

    for (int gen=0; gen<iNumBestSweepers; ++gen)
    {
        m_vecBestGenomes.push_back(tempVec[gen]);
    }
}

//----------------- GetBestPhenotypesFromLastGeneration ------------------
//
//  returns a std::vector of the n best phenotypes from the previous
//  generation
//------------------------------------------------------------------------
vector<NEATNeuralNet*> NEATGA::GetBestPhenotypesFromLastGeneration()
{
  vector<NEATNeuralNet*> brains;

  for (int gen=0; gen<m_vecBestGenomes.size(); ++gen)
  {
    //calculate max network depth
    int depth = CalculateNetDepth(m_vecBestGenomes[gen]);

    brains.push_back(m_vecBestGenomes[gen].CreatePhenotype(depth));
  }

  return brains;
}

//--------------------------- AdjustSpecies ------------------------------
//
//  this functions simply iterates through each species and calls
//  AdjustFitness for each species
//------------------------------------------------------------------------
void NEATGA::AdjustSpeciesFitnesses()
{
  for (int sp=0; sp<m_vecSpecies.size(); ++sp)
  {
    m_vecSpecies[sp].AdjustFitnesses();
  }
}

//------------------ SpeciateAndCalculateSpawnLevels ---------------------
//
//  separates each individual into its respective species by calculating
//  a compatibility score with every other member of the population and
//  niching accordingly. The function then adjusts the fitness scores of
//  each individual by species age and by sharing and also determines
//  how many offspring each individual should spawn.
//------------------------------------------------------------------------
void NEATGA::SpeciateAndCalculateSpawnLevels(bool isRealTime)
{
  bool bAdded = false;
  int gen = 0;

  if(isRealTime)
    m_vecSpecies.clear();

  //iterate through each genome and speciate
  for (gen=0; gen<m_vecGenomes.size(); ++gen)
  {
    if(!m_vecGenomes[gen].isActive())
        continue;

    //calculate its compatibility score with each species leader. If
    //compatible add to species. If not, create a new species
    for (int spc=0; spc<m_vecSpecies.size(); ++spc)
    {
      double compatibility = m_vecGenomes[gen].GetCompatibilityScore(m_vecSpecies[spc].Leader());

      //if this individual is similar to this species add to species
      if (compatibility <= dCompatibilityThreshold)
      {
        m_vecSpecies[spc].AddMember(m_vecGenomes[gen]);

        //let the genome know which species it's in
        m_vecGenomes[gen].SetSpecies(m_vecSpecies[spc].ID());

        bAdded = true;

        break;
      }
    }

    if (!bAdded)
    {
      //we have not found a compatible species so let's create a new one
      m_vecSpecies.push_back(Species(m_vecGenomes[gen], m_iNextSpeciesID++));
    }

    bAdded = false;

  }

  //now all the genomes have been assigned a species the fitness scores
  //need to be adjusted to take into account sharing and species age.
  AdjustSpeciesFitnesses();

      mWorstAdjIndvIndex = -1;
    double worstFitness = 9999;

  //calculate new adjusted total & average fitness for the population
  for (gen=0; gen<m_vecGenomes.size(); ++gen)
  {
    if(!m_vecGenomes[gen].isActive())
        continue;

     m_dTotFitAdj += m_vecGenomes[gen].GetAdjFitness();

     if(m_vecGenomes[gen].GetAdjFitness() < worstFitness
    && m_vecGenomes[gen].getLifeTick() > MinTicks)
    {
        mWorstAdjIndvIndex = gen;
        worstFitness = m_vecGenomes[gen].GetAdjFitness();
    }
  }

  m_dAvFitAdj = m_dTotFitAdj/m_vecGenomes.size();

  //calculate how many offspring each member of the population
  //should spawn
  for (gen=0; gen<m_vecGenomes.size(); ++gen)
  {
     if(!m_vecGenomes[gen].isActive())
        continue;

     double ToSpawn = m_vecGenomes[gen].GetAdjFitness() / m_dAvFitAdj;

     m_vecGenomes[gen].SetAmountToSpawn(ToSpawn);
  }

  //iterate through all the species and calculate how many offspring
  //each species should spawn
  for (int spc=0; spc<m_vecSpecies.size(); ++spc)
  {
    m_vecSpecies[spc].CalculateSpawnAmount();
  }

}

//--------------------------- TournamentSelection ------------------------
//
//------------------------------------------------------------------------
NEATGenotype NEATGA::TournamentSelection(const int NumComparisons)
{
   double BestFitnessSoFar = 0;

   int ChosenOne = 0;

   //Select NumComparisons members from the population at random testing
   //against the best found so far
   for (int i=0; i<NumComparisons; ++i)
   {
     int ThisTry = rangedInt(0, m_vecGenomes.size()-1);

     if (m_vecGenomes[ThisTry].Fitness() > BestFitnessSoFar)
     {
       ChosenOne = ThisTry;

       BestFitnessSoFar = m_vecGenomes[ThisTry].Fitness();
     }
   }

   //return the champion
   return m_vecGenomes[ChosenOne];
}

//-----------------------------------Crossover----------------------------
//
//------------------------------------------------------------------------
NEATGenotype NEATGA::Crossover(NEATGenotype& mum, NEATGenotype& dad)
{

  //helps make the code clearer
  enum parent_type{MUM, DAD,};

  //first, calculate the genome we will using the disjoint/excess
  //genes from. This is the fittest genome.
  parent_type best;

  //if they are of equal fitness use the shorter (because we want to keep
  //the networks as small as possible)
  if (mum.Fitness() == dad.Fitness())
  {
    //if they are of equal fitness and length just choose one at
    //random
    if (mum.NumGenes() == dad.NumGenes())
    {
      best = (parent_type)rangedInt(0, 1);
    }

    else
    {
      if (mum.NumGenes() < dad.NumGenes())
      {
        best = MUM;
      }

      else
      {
        best = DAD;
      }
    }
  }

  else
  {
    if (mum.Fitness() > dad.Fitness())
    {
      best = MUM;
    }

    else
    {
      best = DAD;
    }
  }

  //these vectors will hold the offspring's nodes and genes
  vector<SNeuronGene>  BabyNeurons;
  vector<SLinkGene>    BabyGenes;

  //temporary vector to store all added node IDs
  vector<int> vecNeurons;

  //create iterators so we can step through each parents genes and set
  //them to the first gene of each parent
  vector<SLinkGene>::iterator curMum = mum.StartOfGenes();
  vector<SLinkGene>::iterator curDad = dad.StartOfGenes();

  //this will hold a copy of the gene we wish to add at each step
  SLinkGene SelectedGene;

  //step through each parents genes until we reach the end of both
  while (!((curMum == mum.EndOfGenes()) && (curDad == dad.EndOfGenes())))
  {

    //the end of mum's genes have been reached
    if ((curMum == mum.EndOfGenes())&&(curDad != dad.EndOfGenes()))
    {
      //if dad is fittest
      if (best == DAD)
      {
        //add dads genes
        SelectedGene = *curDad;
      }

      //move onto dad's next gene
      ++curDad;
    }

    //the end of dads's genes have been reached
    else if ( (curDad == dad.EndOfGenes()) && (curMum != mum.EndOfGenes()))
    {
      //if mum is fittest
      if (best == MUM)
      {
        //add mums genes
        SelectedGene = *curMum;
      }

      //move onto mum's next gene
      ++curMum;
    }

    //if mums innovation number is less than dads
    else if (curMum->InnovationID < curDad->InnovationID)
    {
      //if mum is fittest add gene
      if (best == MUM)
      {
        SelectedGene = *curMum;
      }

      //move onto mum's next gene
      ++curMum;
    }

    //if dads innovation number is less than mums
    else if (curDad->InnovationID < curMum->InnovationID)
    {
      //if dad is fittest add gene
      if (best = DAD)
      {
        SelectedGene = *curDad;
      }

      //move onto dad's next gene
      ++curDad;
    }

    //if innovation numbers are the same
    else if (curDad->InnovationID == curMum->InnovationID)
    {
      //grab a gene from either parent
      if (randFloat() < 0.5f)
      {
        SelectedGene = *curMum;
      }

      else
      {
        SelectedGene = *curDad;
      }

      //move onto next gene of each parent
      ++curMum;
      ++curDad;
    }

    //add the selected gene if not already added
    if (BabyGenes.size() == 0)
    {
      BabyGenes.push_back(SelectedGene);
    }

    else
    {
      if (BabyGenes[BabyGenes.size()-1].InnovationID !=
          SelectedGene.InnovationID)
      {
        BabyGenes.push_back(SelectedGene);
      }
    }

    //Check if we already have the nodes referred to in SelectedGene.
    //If not, they need to be added.
    AddNeuronID(SelectedGene.FromNeuron, vecNeurons);
    AddNeuronID(SelectedGene.ToNeuron, vecNeurons);

  }//end while

  //now create the required nodes. First sort them into order
  sort(vecNeurons.begin(), vecNeurons.end());

  for (int i=0; i<vecNeurons.size(); i++)
  {
    BabyNeurons.push_back(m_pInnovation->CreateNeuronFromID(vecNeurons[i]));
  }

  //finally, create the genome
  NEATGenotype babyGenome(m_iNextGenomeID++,
                     BabyNeurons,
                     BabyGenes,
                     mum.NumInputs(),
                     mum.NumOutputs());

  return babyGenome;
}


//--------------------------- ResetAndKill -------------------------------
//
//  This function resets some values ready for the next epoch, kills off
//  all the phenotypes and any poorly performing species.
//------------------------------------------------------------------------
void NEATGA::ResetAndKill(bool isRealTime)
{
  m_dTotFitAdj = 0;
  m_dAvFitAdj  = 0;

  //purge the species
  vector<Species>::iterator curSp = m_vecSpecies.begin();

  while (curSp != m_vecSpecies.end())
  {
    curSp->Purge();

    //kill off species if not improving and if not the species which contains
    //the best genome found so far
    if ( (curSp->GensNoImprovement() > iNumGensAllowedNoImprovement) &&
         (curSp->BestFitness() < m_dBestEverFitness) )
    {
     curSp = m_vecSpecies.erase(curSp);
     --curSp;
    }

    ++curSp;
  }

    if(!isRealTime)
    {
          //we can also delete the phenotypes
          for (int gen=0; gen<m_vecGenomes.size(); ++gen)
          {
            m_vecGenomes[gen].DeletePhenotype();
          }
    }
}

//------------------------------- Split ----------------------------------
//
//  this function is used to create a lookup table that is used to
//  calculate the depth of the network.
//------------------------------------------------------------------------
vector<SplitDepth> NEATGA::Split(double low, double high, int depth)
{
  static vector<SplitDepth> vecSplits;

  double span = high-low;

  vecSplits.push_back(SplitDepth(low + span/2, depth+1));

  if (depth > 6)
  {
    return vecSplits;
  }

  else
  {
    Split(low, low+span/2, depth+1);
    Split(low+span/2, high, depth+1);

    return vecSplits;
  }
}



