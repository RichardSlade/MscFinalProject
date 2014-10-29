#ifndef INCL_UTILITY_HPP
#define INCL_UTILITY_HPP

#include <cmath>
#include <fstream>

#include <SFML/System/Vector2.hpp>
#include <Enum.hpp>

//#include <Constants.hpp>
//#include <NEATGenes.hpp>

static void Clamp(double &arg, double min, double max)
{
    if(min > max
       || max < min)
    {
        int temp = max;
        max = min;
        min = temp;
    }

	if (arg < min)
	{
		arg = min;
	}

	if (arg > max)
	{
		arg = max;
	}
}

static inline float randFloat()
{
    return (rand() / (RAND_MAX + 1.0));
}

////clamps the first argument between the second two
//inline void Clamp(double &arg, double min, double max)
//{
//	if (arg < min)
//	{
//		arg = min;
//	}
//
//	if (arg > max)
//	{
//		arg = max;
//	}
//}


//returns a random float in the range -1 < n < 1
static inline double RandomClamped()
{
    return randFloat() - randFloat();
}

static inline int rangedInt(int lo,int hi)
{
    return lo + static_cast<int> (rand()) / (static_cast<int> (RAND_MAX/(hi-lo)));
}

static double rangedDouble(int lo, int hi)
{
    return lo + static_cast<double> (rand()) / (static_cast<double> (RAND_MAX/(hi-lo)));
}

static float rangedFloat(int lo, int hi)
{
    return lo + static_cast<float> (rand()) / (static_cast<float> (RAND_MAX/(hi-lo)));
}

static double vecMag(sf::Vector2f vec)
{
    return std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
}

static void vecNorm(sf::Vector2f& vec)
{
    float mag = vecMag(vec);

    vec = vec / mag;
}

//inline double Vec2DLength(const SVector2D &v)
//{
//	return sqrt(v.x * v.x + v.y * v.y);
//}
//
//inline void Vec2DNormalize(SVector2D &v)
//{
//	double vector_length = Vec2DLength(v);
//
//	v.x = v.x / vector_length;
//	v.y = v.y / vector_length;
//}

static inline double vecDot(sf::Vector2f v1, sf::Vector2f v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

inline int vecSign(sf::Vector2f v1, sf::Vector2f v2)
{
  if (v1.y*v2.x > v1.x*v2.y)
  {
    return 1;
  }
  else
  {
    return -1;
  }
}

static bool lineIntersection(const sf::Vector2f A, const sf::Vector2f B, const sf::Vector2f C, const sf::Vector2f D, double &dist)
{
    double rTop = (A.y-C.y)*(D.x-C.x)-(A.x-C.x)*(D.y-C.y);
	double rBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);

	double sTop = (A.y-C.y)*(B.x-A.x)-(A.x-C.x)*(B.y-A.y);
	double sBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);

	if ( (rBot == 0) || (sBot == 0))
	{
		//lines are parallel
		return false;
	}

	double r = rTop/rBot;
	double s = sTop/sBot;

	if( (r > 0) && (r < 1.0f) && (s > 0) && (s < 1.0f) )
    {
        dist = r;
        return true;
    }
	else
    {
		dist = 0;
        return false;
    }
}

//rounds a double up or down depending on its value
inline int Rounded(double val)
{
  int    integral = (int)val;
  double mantissa = val - integral;

  if (mantissa < 0.5)
  {
    return integral;
  }

  else
  {
    return integral + 1;
  }
}

static bool fileExist(Ai::Type aiType)
{
    std::string fileName;

    switch(aiType)
    {
        case Ai::Harvester: fileName = "brains/Harvester/neurons/0neurons.dat"; break;
        case Ai::Hunter: fileName = "brains/Hunter/neurons/0neurons.dat"; break;
    }

    std::ifstream inFile(fileName);

    return inFile;
}

#endif // INCL_UTILITY_HPP
