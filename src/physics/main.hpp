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

	void   tick();
	double getDeltaTime() const;

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

enum BoundingBoxType { Sphere };

class BoundingBox {
  protected:
	float           m_restitutionCoef;
	float           m_sliding;
	glm::vec3       m_position;
	BoundingBoxType m_type;

  public:
	BoundingBox(float restitutionCoef, float sliding, glm::vec3 position, BoundingBoxType type);

	float           getRestitutionCoef() const;
	float           getSliding() const;
	glm::vec3       getPosition() const;
	BoundingBoxType getType() const;
	virtual float   getRadius() const;
	virtual Force intersect(BoundingBox const& boundingBox, glm::vec3 hisTranslation, UnitQuaternion hisRotation, glm::vec3 thisTranslation,
	                        UnitQuaternion thisRotation, glm::vec3 thisSpeed) const = 0;

	~BoundingBox();
};

// L'ensemble des calculs effectués dans cette classe se feront dans le repère local de l'objet
class Solid {
  protected:
	std::vector<Mass> m_masses;
	glm::vec3         m_inertiaCenter;
	glm::mat3         m_inertiaTensor;
	bool              m_locked;
	glm::vec3         m_speedVector;
	glm::vec3         m_angularMomentum;

  public:
	Solid(std::vector<Mass> masses = std::vector<Mass>(), bool locked = false);

	// getters
	bool      getLocked() const;
	float     getTotalMass() const;
	glm::vec3 getInertiaCenter() const;
	glm::mat3 getInertiaTensor() const;
	glm::vec3 getSpeedVector() const;
	glm::vec3 getAngularMomentum() const;
	glm::vec3 getAngularSpeed() const;
	glm::vec3 getSpeedAt(glm::vec3 point) const;

	// memoization
	void calculateInertiaCenter();
	void calculateInertiaTensor();

	// setters
	Solid& setSpeedVector(glm::vec3 speedVector);
	Solid& setAngularMomentum(glm::vec3 angularMomentum);
	Solid& incrementSpeed(glm::vec3 deltaSpeed);
	Solid& incrementAngularMomentum(glm::vec3 deltaAngularMomentum);
	Solid& applyLinearImpulse(glm::vec3 impulse);
	Solid& applyAngularImpulse(glm::vec3 impulse);

	~Solid();
};

// Travail dans le repère monde
class WorldObject {
  private:
	std::vector<BoundingBox*> m_boundingBoxes;
	Solid&                    m_solid;
	Mesh&                     m_mesh;

	glm::vec3 m_resultantForce;  // dans le repère monde
	glm::vec3 m_torque;          // Par rapport au centre de masse, dans le repère monde

  public:
	WorldObject(std::vector<BoundingBox*> boundingBoxes, Solid& solid, Mesh& mesh);

	glm::vec3    getTorque() const;
	glm::vec3    getResultantForce() const;
	WorldObject& applyForce(Force const& force);       // force : pt d'application dans le repère local et direction dans le repère monde
	glm::mat2x3  getWrench(Force const& force) const;  // force de la même nature que précisé précédemment
	WorldObject& applyWrench(glm::mat2x3 wrench, glm::vec3 point);  // wrench dans le repère monde et point dans le repère local

	std::vector<BoundingBox*>& getBoundingBoxes();
	Solid&                     getSolid();
	Mesh&                      getMesh();
	WorldObject&               update(double deltaTime);

	~WorldObject();
};

class Joint {
  protected:
	WorldObject* m_worldObject1;
	glm::vec3    m_wO1Contact;
	WorldObject* m_worldObject2;
	glm::vec3    m_wO2Contact;
	glm::mat2x3  m_twist;

  public:
	Joint(WorldObject* worldObject1, glm::vec3 wO1Contact, WorldObject* worldObject2, glm::vec3 wO2Contact);

	WorldObject* getWorldObject1();
	WorldObject* getWorldObject2();
	glm::mat2x3& getTwist();
	virtual void applyConstraints(double deltaTime) = 0;

	~Joint();
};

class Skeleton {
  private:
	std::vector<WorldObject*> m_worldObjects;
	std::vector<Joint*>       m_joints;

  public:
	Skeleton(std::vector<WorldObject*> worldObjects, std::vector<Joint*> joints);

	std::vector<Joint*>& getJoints();
	void                 update(double deltaTime);

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


// Les BoundingBox

class SphereBoundingBox : public BoundingBox {
  private:
	float m_radius;

  public:
	SphereBoundingBox(glm::vec3 position = glm::vec3(), float radius = 1, float restitutionCoef = 1, float sliding = 1);

	float getRadius() const;
	Force intersect(BoundingBox const& boundingBox, glm::vec3 hisTranslation, UnitQuaternion hisRotation, glm::vec3 thisTranslation,
	                UnitQuaternion thisRotation, glm::vec3 thisSpeed) const;

	~SphereBoundingBox();
};


// Les liaisons

class BallJoint : public Joint {
  public:
	BallJoint(WorldObject* worldObject1, glm::vec3 wO1Contact, WorldObject* worldObject2, glm::vec3 wO2Contact);

	void applyConstraints(double deltaTime);

	~BallJoint();
};

#endif
