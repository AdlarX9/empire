#ifndef PHYSICS
#define PHYSICS

#include "../maths/utils.hpp"
#include "../three/main.hpp"
#include "../lib/glad/glad.h"
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

class Clock {
  private:
	long long                                                   m_deltaTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_now;

  public:
	Clock();

	void      tick();
	long long getDeltaTime() const;

	~Clock();
};

class Force {
  private:
	glm::vec3 m_position;
	glm::vec3 m_direction;

  public:
	Force(glm::vec3 position = glm::vec3(0), glm::vec3 direction = glm::vec3(0));

	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;

	~Force();
};

class Mass {
  private:
	float     m_mass;
	glm::vec3 m_position;

  public:
	Mass(float mass, glm::vec3 position);

	float     getMass() const;
	glm::vec3 getPosition() const;

	~Mass();
};

enum BoundingBoxType { Box, Sphere, Cylinder };

class BoundingBox {
  protected:
	float           m_restitutionCoef;
	float           m_sliding;
	BoundingBoxType m_type;

  public:
	BoundingBox(float restitutionCoef, float sliding, BoundingBoxType type);

	virtual Force intersect(BoundingBox const& boundingBox) const = 0;

	~BoundingBox();
};

class Solid {
  protected:
	std::vector<Mass> m_masses;
	glm::vec3         m_inertiaCenter;
	glm::mat3         m_inertiaTensor;
	glm::vec3         m_speedVector;
	UnitQuaternion    m_angularMomentum;
	bool              m_locked;

  public:
	Solid(std::vector<Mass> masses = std::vector<Mass>(), bool locked = false);

	float          getTotalMass() const;
	glm::vec3      getInertiaCenter() const;
	glm::mat3      getInertiaTensor() const;
	glm::vec3      getSpeedVector() const;
	UnitQuaternion getAngularMomentum() const;
	void           calculateInertiaCenter();
	void           calculateInertiaTensor();
	Solid&         applyForce(Force const& force);

	~Solid();
};

class WorldObject {
  private:
	BoundingBox& m_boundingBox;
	Solid&       m_solid;
	Mesh&        m_mesh;

  public:
	WorldObject(BoundingBox& boundingBox, Solid& solid, Mesh& mesh);

	Mesh&        getMesh();
	WorldObject& update(long long deltaTime);

	~WorldObject();
};

class Joint {
  private:
	WorldObject* m_worldObject1;
	WorldObject* m_worldObject2;

  public:
	Joint(WorldObject* worldObject1, WorldObject* worldObject2);

	WorldObject* getWorldObject1();
	WorldObject* getWorldObject2();
	void         update(long long deltaTime);

	~Joint();
};

class Skeleton {
  private:
	std::vector<Joint*> m_joints;

  public:
	Skeleton(std::vector<Joint*> joints);

	std::vector<Joint*>& getJoints();
	void                 update(long long deltaTime);

	~Skeleton();
};

class Planet {
  private:
	Clock                  m_clock;
	float                  m_gravityIntensity;
	Scene                  m_scene;
	std::vector<Skeleton*> m_skeletons;

  public:
	Planet(float gravityIntensity = 9.81);

	Scene&  getScene();
	Planet& add(Skeleton* skeleton);
	Planet& remove(Skeleton* skeleton);
	void    update();

	~Planet();
};

#endif
