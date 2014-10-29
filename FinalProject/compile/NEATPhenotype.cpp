#include <NEATPhenotype.hpp>

//------------------------------------------------------------------------
//
//	Name: NEATPhenotype.cpp
//
//  Author: Mat Buckland 2002
//
//------------------------------------------------------------------------

//------------------------------------Sigmoid function------------------------
//
//----------------------------------------------------------------------------

float Sigmoid(float netinput, float response)
{
	return ( 1 / ( 1 + exp(-netinput / response)));
}


//--------------------------------- ctor ---------------------------------
//
//------------------------------------------------------------------------
NEATNeuralNet::NEATNeuralNet(vector<SNeuron*> neurons,
                       int              depth):m_vecpNeurons(neurons),
                                               m_iDepth(depth),
                                               mNetworkLinks(sf::Lines),
                                               mNetworkNeurons(sf::Points)
{}


//--------------------------------- dtor ---------------------------------
//
//------------------------------------------------------------------------
NEATNeuralNet::~NEATNeuralNet()
{
  //delete any live neurons
  for (int i=0; i<m_vecpNeurons.size(); ++i)
  {
    if (m_vecpNeurons[i])
    {
      delete m_vecpNeurons[i];

      m_vecpNeurons[i] = NULL;
    }
  }
}

//----------------------------------Update--------------------------------
//	takes a list of doubles as inputs into the network then steps through
//  the neurons calculating each neurons next output.
//
//	finally returns a std::vector of doubles as the output from the net.
//------------------------------------------------------------------------
vector<double> NEATNeuralNet::Update(const vector<double> &inputs)
{
  //create a vector to put the outputs into
  vector<double>	outputs;

  //if the mode is snapshot then we require all the neurons to be
  //iterated through as many times as the network is deep. If the
  //mode is set to active the method can return an output after
  //just one iteration
  int FlushCount = 0;

    run_type type = active;

  if (type == snapshot)
  {
    FlushCount = m_iDepth;
  }
  else
  {
    FlushCount = 1;
  }

  //iterate through the network FlushCount times
  for (int i=0; i<FlushCount; ++i)
  {
    //clear the output vector
    outputs.clear();

    //this is an index into the current neuron
    int cNeuron = 0;

    //first set the outputs of the 'input' neurons to be equal
    //to the values passed into the function in inputs
    while (m_vecpNeurons[cNeuron]->NeuronType == input)
    {
      m_vecpNeurons[cNeuron]->dOutput = inputs[cNeuron];

      ++cNeuron;

       if(m_vecpNeurons.size() - 1 < cNeuron)
        throw std::runtime_error("loop");
    }

    //set the output of the bias to 1
    m_vecpNeurons[cNeuron++]->dOutput = 1;

    //then we step through the network a neuron at a time
    while (cNeuron < m_vecpNeurons.size())
    {
      //this will hold the sum of all the inputs x weights
      double sum = 0;

      //sum this neuron's inputs by iterating through all the links into
      //the neuron
      for (int lnk=0; lnk<m_vecpNeurons[cNeuron]->vecLinksIn.size(); ++lnk)
      {
        //get this link's weight
        double Weight = m_vecpNeurons[cNeuron]->vecLinksIn[lnk].dWeight;

        //get the output from the neuron this link is coming from
        double NeuronOutput =
        m_vecpNeurons[cNeuron]->vecLinksIn[lnk].pIn->dOutput;

        //add to sum
        sum += Weight * NeuronOutput;
      }

      //now put the sum through the activation function and assign the
      //value to this neuron's output
      m_vecpNeurons[cNeuron]->dOutput =
      Sigmoid(sum, m_vecpNeurons[cNeuron]->dActivationResponse);

      if (m_vecpNeurons[cNeuron]->NeuronType == output)
      {
        //add to our outputs
        outputs.push_back(m_vecpNeurons[cNeuron]->dOutput);
      }

      //next neuron
      ++cNeuron;
    }

  }//next iteration through the network

  //the network needs to be flushed if this type of update is performed
  //otherwise it is possible for dependencies to be built on the order
  //the training data is presented
  if (type == snapshot)
  {
    for (int n=0; n<m_vecpNeurons.size(); ++n)
    {
      m_vecpNeurons[n]->dOutput = 0;
    }
  }

  //return the outputs
  return outputs;
}

//----------------------------- TidyXSplits -----------------------------
//
//  This is a fix to prevent neurons overlapping when they are displayed
//-----------------------------------------------------------------------
void TidyXSplits(vector<SNeuron*> &neurons)
{
  //stores the index of any neurons with identical splitY values
  vector<int>    SameLevelNeurons;

  //stores all the splitY values already checked
  vector<double> DepthsChecked;


  //for each neuron find all neurons of identical ySplit level
  for (int n=0; n<neurons.size(); ++n)
  {
    double ThisDepth = neurons[n]->dSplitY;

    //check to see if we have already adjusted the neurons at this depth
    bool bAlreadyChecked = false;

    for (int i=0; i<DepthsChecked.size(); ++i)
    {
      if (DepthsChecked[i] == ThisDepth)
      {
        bAlreadyChecked = true;

        break;
      }
    }

    //add this depth to the depths checked.
    DepthsChecked.push_back(ThisDepth);

    //if this depth has not already been adjusted
    if (!bAlreadyChecked)
    {
      //clear this storage and add the neuron's index we are checking
      SameLevelNeurons.clear();
      SameLevelNeurons.push_back(n);

      //find all the neurons with this splitY depth
      for (int i=n+1; i<neurons.size(); ++i)
      {
        if (neurons[i]->dSplitY == ThisDepth)
        {
          //add the index to this neuron
          SameLevelNeurons.push_back(i);
        }
      }

      //calculate the distance between each neuron
      double slice = 1.0/(SameLevelNeurons.size()+1);


      //separate all neurons at this level
      for (int i=0; i<SameLevelNeurons.size(); ++i)
      {
        int idx = SameLevelNeurons[i];

        neurons[idx]->dSplitX = (i+1) * slice;
      }
    }

  }//next neuron to check

}

//----------------------------- DrawNet ----------------------------------
//
//  creates a representation of the ANN on a device context
//
//------------------------------------------------------------------------
void NEATNeuralNet::DrawNet(int Left, int Right, int Top, int Bottom)
{
  //the border width
  const int border = 10;

  //max line thickness
  const int MaxThickness = 5;

  TidyXSplits(m_vecpNeurons);

  //go through the neurons and assign x/y coords
  int spanX = Right - Left;
  int spanY = Top - Bottom - (2*border);

  for (int cNeuron=0; cNeuron<m_vecpNeurons.size(); ++cNeuron)
  {
    m_vecpNeurons[cNeuron]->iPosX = Left + spanX*m_vecpNeurons[cNeuron]->dSplitX;
    m_vecpNeurons[cNeuron]->iPosY = (Top - border) - (spanY * m_vecpNeurons[cNeuron]->dSplitY);
  }

    mNetworkLinks.clear();
    mNetworkNeurons.clear();
    mNeuronCircles.clear();
    std::vector<int> prevIDs;

    for(int cNeuron=0; cNeuron<m_vecpNeurons.size(); ++cNeuron)
    {
        bool alreadyAdded = false;

        for(int& i : prevIDs)
        {
            if(m_vecpNeurons[cNeuron]->iNeuronID == i)
            {
                alreadyAdded = true;
                break;
            }
        }

        if(alreadyAdded)
            continue;

        prevIDs.push_back(m_vecpNeurons[cNeuron]->iNeuronID);

        mNetworkNeurons.append(sf::Vertex(sf::Vector2f(m_vecpNeurons[cNeuron]->iPosX, m_vecpNeurons[cNeuron]->iPosY), sf::Color::Red));

        sf::CircleShape c(5.f);
        sf::FloatRect bounds = c.getLocalBounds();

        c.setOrigin(bounds.width / 2.f, bounds.height /2.f);
        c.setPosition(m_vecpNeurons[cNeuron]->iPosX, m_vecpNeurons[cNeuron]->iPosY);
        c.setFillColor(sf::Color::Red);
        mNeuronCircles.push_back(c);

        mNetworkLinks.append(sf::Vertex(sf::Vector2f(m_vecpNeurons[cNeuron]->iPosX, m_vecpNeurons[cNeuron]->iPosY), sf::Color::Green));

        std::vector<SLink> links = m_vecpNeurons[cNeuron]->vecLinksIn;

        for(SLink& link : links)
        {
            alreadyAdded = false;
            int id = link.pIn->iNeuronID;
            bool isInput = true;

            if(id == m_vecpNeurons[cNeuron]->iNeuronID)
            {
                id = link.pOut->iNeuronID;
                isInput = false;
            }

//            for(int& i : prevIDs)
//            {
//                if(id == i)
//                {
//                    alreadyAdded = true;
//                    break;
//                }
//            }
//
//            if(alreadyAdded)
//                continue;
//
//            prevIDs.push_back(id);

            SNeuron* n = nullptr;

            if(isInput)
                n = link.pIn;
            else
                n = link.pOut;

            mNetworkNeurons.append(sf::Vertex(sf::Vector2f(n->iPosX, n->iPosY), sf::Color::Red));

            sf::CircleShape c(5.f);

        bounds = c.getLocalBounds();

            c.setOrigin(bounds.width / 2.f, bounds.height /2.f);
            c.setPosition(n->iPosX, n->iPosY);
            c.setFillColor(sf::Color::Red);
            mNeuronCircles.push_back(c);

            mNetworkLinks.append(sf::Vertex(sf::Vector2f(n->iPosX, n->iPosY), sf::Color::Green));
            mNetworkLinks.append(sf::Vertex(sf::Vector2f(m_vecpNeurons[cNeuron]->iPosX, m_vecpNeurons[cNeuron]->iPosY), sf::Color::Green));
        }

        links.clear();
        links = m_vecpNeurons[cNeuron]->vecLinksOut;

        for(SLink& link : links)
        {
            alreadyAdded = false;
            int id = link.pIn->iNeuronID;
            bool isInput = true;

            if(id == m_vecpNeurons[cNeuron]->iNeuronID)
            {
                id = link.pOut->iNeuronID;
                isInput = false;
            }

//            for(int& i : prevIDs)
//            {
//                if(id == i)
//                {
//                    alreadyAdded = true;
//                    break;
//                }
//            }
//
//            if(alreadyAdded)
//                continue;
//
//            prevIDs.push_back(id);

            SNeuron* n = nullptr;

            if(isInput)
                n = link.pIn;
            else
                n = link.pOut;

            mNetworkNeurons.append(sf::Vertex(sf::Vector2f(n->iPosX, n->iPosY), sf::Color::Red));

            sf::CircleShape c(5.f);
            bounds = c.getLocalBounds();
            c.setOrigin(bounds.width / 2.f, bounds.height /2.f);
            c.setPosition(n->iPosX, n->iPosY);
            c.setFillColor(sf::Color::Red);
            mNeuronCircles.push_back(c);

            mNetworkLinks.append(sf::Vertex(sf::Vector2f(n->iPosX, n->iPosY), sf::Color::Green));
            mNetworkLinks.append(sf::Vertex(sf::Vector2f(m_vecpNeurons[cNeuron]->iPosX, m_vecpNeurons[cNeuron]->iPosY), sf::Color::Green));
        }
    }


}




