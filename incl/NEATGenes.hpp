#ifndef NEATGENES_HPP
#define NEATGENES_HPP
//-----------------------------------------------------------------------
//
//  Name: GeneGenes.h
//
//  Author: Mat Buckland 2002
//
//	Desc: neuron and link gene definitions used in the
//        implementation of Kenneth Owen Stanley's and Risto
//        Miikkulainen's NEAT idea.These structures are used to define
//        a genome.
//
//-----------------------------------------------------------------------

class CNeuron;

//------------------------------------------------------------------------
//
//  enumeration for all the available neuron types
//------------------------------------------------------------------------
enum neuron_type
{
  input,
  hidden,
  output,
  bias,
  none
};

//------------------------------------------------------------------------
//
//  this structure defines a neuron gene
//------------------------------------------------------------------------
struct SNeuronGene
{
  //its identification
  int         iID;

  //its type
  neuron_type NeuronType;
  int       NeuronTypeInt;

  //is it recurrent
  bool        bRecurrent;

  //sets the curvature of the sigmoid function
  double    dActivationResponse;

  //position in network grid
  double    dSplitY, dSplitX;

  SNeuronGene(neuron_type type,
              int         id,
              double      y,
              double      x,
              bool        r = false):iID(id),
                                     NeuronType(type),
                                     bRecurrent(r),
                                     dSplitY(y),
                                     dSplitX(x),
                                     dActivationResponse(1)
  {}

  SNeuronGene(){};

  // Insertion operator
    friend std::ostream& operator<<(std::ostream& os, const SNeuronGene& nG)
    {
        // write out individual members of s with an end of line between each one
        os << nG.iID << '\n';
        os << nG.NeuronTypeInt << '\n';
        os << nG.bRecurrent << '\n';
        os << nG.dActivationResponse << '\n';
        os << nG.dSplitY << '\n';
        os << nG.dSplitX << '\n';

//        NeuronType = (neuron_type)nG.NeuronTypeInt;

        return os;
    }

    // Extraction operator
    friend std::istream& operator>>(std::istream& is, SNeuronGene& nG)
    {
        // read in individual members of s
        is
        >> nG.iID
        >> nG.NeuronTypeInt
        >> nG.bRecurrent
        >> nG.dActivationResponse
        >> nG.dSplitY
        >> nG.dSplitX;

        return is;
    }
};

//------------------------------------------------------------------------
//
//  this structure defines a link gene
//------------------------------------------------------------------------
struct SLinkGene
{
	//the IDs of the two neurons this link connects
	int     FromNeuron,
	        ToNeuron;

	double	dWeight;

  //flag to indicate if this link is currently enabled or not
  bool    bEnabled;

  //flag to indicate if this link is recurrent or not
  bool    bRecurrent;

	int     InnovationID;

  SLinkGene(){}

  SLinkGene(int    in,
            int    out,
            bool   enable,
            int    tag,
            double w,
            bool   rec = false):bEnabled(enable),
								                InnovationID(tag),
                                FromNeuron(in),
                                ToNeuron(out),
                                dWeight(w),
                                bRecurrent(rec)
	{}

  //overload '<' used for sorting(we use the innovation ID as the criteria)
	friend bool operator<(const SLinkGene& lhs, const SLinkGene& rhs)
	{
		return (lhs.InnovationID < rhs.InnovationID);
	}

	 // Insertion operator
    friend std::ostream& operator<<(std::ostream& os, const SLinkGene& lG)
    {
        // write out individual members of s with an end of line between each one
        os << lG.FromNeuron << '\n';
        os << lG.ToNeuron << '\n';
        os << lG.dWeight << '\n';
        os << lG.bEnabled << '\n';
        os << lG.bRecurrent << '\n';
        os << lG.InnovationID << '\n';

        return os;
    }

    // Extraction operator
    friend std::istream& operator>>(std::istream& is, SLinkGene& lG)
    {
        // read in individual members of s
        is
        >> lG.FromNeuron
        >> lG.ToNeuron
        >> lG.dWeight
        >> lG.bEnabled
        >> lG.bRecurrent
        >> lG.InnovationID;
        return is;
    }
};



#endif
