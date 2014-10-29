#include <Particle.hpp>

// Taken From Code Originally from source "Particle System for SFML 2
//  @author Laurent Gomila 2014
//  github.com/LaurentGomila/SFML/wiki/Source%3A-Particle-System-for-SFML-2
// 

ParticleSystem::ParticleSystem(sf::Vector2f startPos) :
  m_dissolve(false),
  m_particleSpeed(50.0f),
  m_transparent(sf::Color(0,0,0,0)),
  m_dissolutionRate(4),
  m_shape(Shape::CIRCLE),
  m_gravity(sf::Vector2f(0.0f,0.0f)),
  mActive(true)
{
  m_startPos = startPos;
}

/************************************************************/
ParticleSystem::~ParticleSystem(void)
{
  /* Clear vector */
  m_particles.clear();
}

/************************************************************/
void ParticleSystem::draw(
  sf::RenderTarget& target, sf::RenderStates states) const
{
  for(const auto& item : m_particles)
    target.draw(&item.get()->drawVertex, 1, sf::Points);
  return;
}

/************************************************************/
void ParticleSystem::fuel(int particles, sf::Color pColour)
{
    mActive = true;

  for(int i = 0; i < particles; i++)
  {
    /* Generate a new particle and put it at the generation point */
    Particle* particle;
    particle = new Particle(sf::seconds(2.f));
    particle->drawVertex.position.x = m_startPos.x;
    particle->drawVertex.position.y = m_startPos.y;

    /* Randomizer initialization */
    std::random_device rd;
    std::mt19937 gen(rd());

    switch(m_shape)
    {
      case Shape::CIRCLE:
      {
        /* Generate a random angle */
        UniRealDist randomAngle(0.0f, (2.0f * 3.14159265));
        float angle = randomAngle(gen);

        /* Use the random angle as a thrust vector for the particle */
        UniRealDist randomAngleCos(0.0f, cos(angle));
        UniRealDist randomAngleSin(0.0f, sin(angle));
        particle->vel.x = randomAngleCos(gen);
        particle->vel.y = randomAngleSin(gen);

        break;
      }
      case Shape::SQUARE:
      {
        /* Square generation */
        UniRealDist randomSide(-1.0f, 1.0f);
        particle->vel.x = randomSide(gen);
        particle->vel.y = randomSide(gen);

        break;
      }
      default:
      {
        particle->vel.x = 0.5f; // Easily detected
        particle->vel.y = 0.5f; // Easily detected
      }
    }

    /* We don't want lame particles. Reject, start over. */
    if(particle->vel.x == 0.0f && particle->vel.y == 0.0f )
    {
      delete particle;
      continue;
    }

    particle->drawVertex.color = pColour;

    m_particles.push_back(ParticlePtr(particle));
  }

  return;
}

/************************************************************/
const std::string ParticleSystem::getNumberOfParticlesString(void) const
{
    std::ostringstream oss;
    oss << m_particles.size();
    return oss.str();
}

/************************************************************/
void ParticleSystem::update(sf::Time dt)
{
   float deltaTime = dt.asSeconds();

    if(m_particles.size() == 0)
        mActive = false;

  /* Run through each particle and apply our system to it */
  for(auto it = m_particles.begin(); it != m_particles.end(); it++)
  {
    /* Apply Gravity */
    (*it)->vel.x += m_gravity.x * deltaTime;
    (*it)->vel.y += m_gravity.y * deltaTime;

    /* Apply thrust */
    (*it)->drawVertex.position.x += (*it)->vel.x * deltaTime * m_particleSpeed;
    (*it)->drawVertex.position.y += (*it)->vel.y * deltaTime * m_particleSpeed;

    /* If they are set to disolve, disolve */
    if(m_dissolve) (*it)->drawVertex.color.a -= m_dissolutionRate;

    (*it)->elapsedTime += dt;

    if((*it)->elapsedTime > (*it)->lifeTime)
    {
        it = m_particles.erase(it);

        if(it == m_particles.end())
            return;
    }

  }

  return;
}
