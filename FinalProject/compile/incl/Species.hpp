#ifndef SPECIES_HPP
#define SPECIES_HPP
//-----------------------------------------------------------------------
//
//  Name: Species.hpp
//
//  Author: Mat Buckland 2002
//
//	Desc: speciazation class used in the implementation of Kenneth Owen
//        Stanley's and Risto Miikkulainen's NEAT idea.
//
//-----------------------------------------------------------------------
#include <vector>
#include <math.h>
#include <iomanip>
#include <iostream>

#include <NEATGenotype.hpp>

using namespace std;


//------------------------------------------------------------------------
//
//  class to hold all the genomes of a given species
//------------------------------------------------------------------------
class Species
{

private:

  //keep a local copy of the first member of this species
  NEATGenotype           m_Leader;

  //pointers to all the genomes within this species
  vector<NEATGenotype*>  m_vecMembers;
  double          mAvgFitness;

  //the species needs an identification number
  int               m_iSpeciesID;

  //best fitness found so far by this species
  double            m_dBestFitness;

  //generations since fitness has improved, we can use
  //this info to kill off a species if required
  int               m_iGensNoImprovement;

  //age of species
  int               m_iAge;

  //how many of this species should be spawned for
  //the next population
  double            m_dSpawnsRqd;


public:

  Species(NEATGenotype &FirstOrg, int SpeciesID);

  //this method boosts the fitnesses of the young, penalizes the
  //fitnesses of the old and then performs fitness sharing over
  //all the members of the species
  void    AdjustFitnesses();

  //adds a new individual to the species
  void    AddMember(NEATGenotype& new_org);

  void    Purge();

  //calculates how many offspring this species should
  void    CalculateSpawnAmount();

  //spawns an individual from the species selected at random
  //from the best CParams::dSurvivalRate percent
  NEATGenotype Spawn();

  double calculateAvgFitness();


  //--------------------------------------accessor methods
  NEATGenotype  Leader()const{return m_Leader;}

  double   NumToSpawn()const{return m_dSpawnsRqd;}

  int      NumMembers()const{return m_vecMembers.size();}

  int      GensNoImprovement()const{return m_iGensNoImprovement;}

  int      ID()const{return m_iSpeciesID;}

  double   SpeciesLeaderFitness()const{return m_Leader.Fitness();}

  double   BestFitness()const{return m_dBestFitness;}

  int      Age()const{return m_iAge;}

  double avgFitness(){ return mAvgFitness; }


  //so we can sort species by best fitness. Largest first
  friend bool operator<(const Species &lhs, const Species &rhs)
  {
    return lhs.m_dBestFitness > rhs.m_dBestFitness;
  }

};

#endif
