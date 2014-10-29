#ifndef INCL_INNOVATION_HPP
#define INCL_INNOVATION_HPP
//-----------------------------------------------------------------------
//
//  Name: Innovation.h
//
//  Author: Mat Buckland 2002
//
//	Desc: class to handle genome innovations used in the implementation
//        of Kenneth Owen Stanley's and Risto Miikkulainen's NEAT idea.
//
//-----------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include <memory>

//#include "utils.h"
//#include <NEATGenotype.hpp>
#include <NEATPhenotype.hpp>
#include <NEATGenes.hpp>

class NEATGenotype;

struct SLinkGene;

//---------------------Innovation related structs/classes----------------
//
//------------------------------------------------------------------------
enum innov_type
{
	new_neuron,
	new_link
};

//------------------------------------------------------------------------
//
//  structure defining an innovation
//------------------------------------------------------------------------
struct	SInnovation
{
  //new neuron or new link?
  innov_type  InnovationType;
  int         InnovationTypeInt;

  int         InnovationID;

  int         NeuronIn;
  int         NeuronOut;

  int         NeuronID;

  neuron_type NeuronType;
  int         NeuronTypeInt;

  //if the innovation is a neuron we need to keep a record
  //of its position in the tree (for display purposes)
  double      dSplitY,
              dSplitX;

    SInnovation(){};

	SInnovation(int        in,
              int        out,
              innov_type t,
              int        inov_id):NeuronIn(in),
                                  NeuronOut(out),
                                  InnovationType(t),
                                  InnovationID(inov_id),
                                  NeuronID(0),
                                  dSplitX(0),
                                  dSplitY(0),
                                  NeuronType(none)
	{}

  SInnovation(SNeuronGene neuron,
              int          innov_id,
              int          neuron_id):InnovationID(innov_id),
                                      NeuronID(neuron_id),
                                      dSplitX(neuron.dSplitX),
                                      dSplitY(neuron.dSplitY),
                                      NeuronType(neuron.NeuronType),
                                      NeuronIn(-1),
                                      NeuronOut(-1)
  {}

  SInnovation(int         in,
              int         out,
              innov_type  t,
              int         inov_id,
              neuron_type type,
              double      x,
              double      y):NeuronIn(in),
                            NeuronOut(out),
                            InnovationType(t),
                            InnovationID(inov_id),
                            NeuronID(0),
                            NeuronType(type),
                            dSplitX(x),
                            dSplitY(y)
	{}

    	 // Insertion operator
    friend std::ostream& operator<<(std::ostream& os, const SInnovation& inno)
    {
        // write out individual members of s with an end of line between each one
        os << inno.InnovationTypeInt << '\n';
        os << inno.InnovationID << '\n';
        os << inno.NeuronIn << '\n';
        os << inno.NeuronOut << '\n';
        os << inno.NeuronID << '\n';
        os << inno.NeuronTypeInt << '\n';
        os << inno.dSplitY << '\n';
        os << inno.dSplitX << '\n';

        return os;
    }

    // Extraction operator
    friend std::istream& operator>>(std::istream& is, SInnovation& inno)
    {
        // read in individual members of s
        is
        >> inno.InnovationTypeInt
        >> inno.InnovationID
        >> inno.NeuronIn
        >> inno.NeuronOut
        >> inno.NeuronID
        >> inno.NeuronTypeInt
        >> inno.dSplitY
        >> inno.dSplitX;

        return is;
    }

};

//------------------------------------------------------------------------
//
//  Innovation class used to keep track of all innovations created during
//  the populations evolution
//------------------------------------------------------------------------
class Innovation
{

//    public:
//        typedef std::unique_ptr<Innovation> InnoPtr;

private:

    std::vector<SInnovation> m_vecInnovs;

    int                 m_NextNeuronID;
    int                 m_NextInnovationNum;


    public:

    Innovation(std::vector<SLinkGene>   start_genes,
              std::vector<SNeuronGene> start_neurons);

    Innovation(){};

    //checks to see if this innovation has already occurred. If it has it
    //returns the innovation ID. If not it returns a negative value.
    int   CheckInnovation(int in, int out, innov_type type);

    //creates a new innovation and returns its ID
    int   CreateNewInnovation(int in, int out, innov_type type);

    //as above but includes adding x/y position of new neuron
    int   CreateNewInnovation(int         from,
                            int         to,
                            innov_type  InnovType,
                            neuron_type NeuronType,
                            double      x,
                            double      y);

    //creates a BasicNeuron from the given neuron ID
    SNeuronGene CreateNeuronFromID(int id);

    int   getNeuronID(){ return m_NextNeuronID; }
    int   getInnoNum(){ return m_NextInnovationNum; }
    std::vector<SInnovation>& getInnovations(){ return m_vecInnovs; }

    void        setNeuronID(int id){ m_NextNeuronID = id; }
    void        setInnoNum(int num){ m_NextInnovationNum = num; }
    void        setInnovations(std::vector<SInnovation> innovations){ m_vecInnovs = innovations; }

    //------------------------------------------------accessor methods
    int   GetNeuronID(int inv)const{return m_vecInnovs[inv].NeuronID;}

    void  Flush(){m_vecInnovs.clear(); return;}

    int   NextNumber(int num = 0)
    {
        m_NextInnovationNum += num;

        return m_NextInnovationNum;
    }
};



#endif
