#include "main.hpp"
#include "../maths/utils.hpp"
#include "../three/main.hpp"
#include "../lib/glad/glad.h"

#include <glm/glm.hpp>
#include <vector>
#include <cmath>
#include <chrono>
#include <iostream>

using namespace std;



/* --- CLOCK --- */



Clock::Clock() : m_deltaTime(1 / 100), m_now(chrono::high_resolution_clock::now()) {}

void Clock::tick() {
	auto newNow = chrono::high_resolution_clock::now();
	m_deltaTime = chrono::duration_cast<chrono::milliseconds>(newNow - m_now).count();
	m_now = newNow;
}

double Clock::getDeltaTime() const { return (double)m_deltaTime / 1000; }

Clock::~Clock() {}



/* --- FORCE --- */



Force::Force(glm::vec3 position, glm::vec3 direction) : m_position(position), m_direction(direction) {}

glm::vec3 Force::getPosition() const { return m_position; }
glm::vec3 Force::getDirection() const { return m_direction; }

Force::~Force() {}



/* --- TORQUE --- */



Torque::Torque(glm::vec3 position, UnitQuaternion rotation) : m_position(position), m_rotation(rotation) {}

glm::vec3      Torque::getPosition() const { return m_position; }
UnitQuaternion Torque::getRotation() const { return m_rotation; }

Torque::~Torque() {}



/* --- MASS --- */



Mass::Mass(float mass, glm::vec3 position) : m_mass(mass), m_position(position) {}

float     Mass::getMass() const { return m_mass; }
glm::vec3 Mass::getPosition() const { return m_position; }

Mass::~Mass() {}



/* --- BOUNDINGBOX --- */



BoundingBox::BoundingBox(float restitutionCoef, float sliding, glm::vec3 position, BoundingBoxType type)
    : m_restitutionCoef(restitutionCoef), m_sliding(sliding), m_position(position), m_type(type) {}


float           BoundingBox::getRestitutionCoef() const { return m_restitutionCoef; }
float           BoundingBox::getSliding() const { return m_sliding; }
glm::vec3       BoundingBox::getPosition() const { return m_position; }
BoundingBoxType BoundingBox::getType() const { return m_type; }
float           BoundingBox::getRadius() const { return 0.0f; }

BoundingBox::~BoundingBox() {}



/* --- SOLID --- */



Solid::Solid(vector<Mass> masses, bool locked)
    : m_masses(masses),
      m_inertiaCenter(glm::vec3(0)),
      m_inertiaTensor(glm::mat3(1)),
      m_resultantForce(glm::vec3(0)),
      m_speedVector(glm::vec3(0)),
      m_torque(glm::vec3(0)),
      m_angularMomentum(glm::vec3(0)),
      m_locked(locked) {
	this->calculateInertiaCenter();
	this->calculateInertiaTensor();
}

float Solid::getTotalMass() const {
	float totalMass(0);

	for (Mass mass : m_masses) {
		totalMass += mass.getMass();
	}

	return totalMass;
}

glm::vec3 Solid::getInertiaCenter() const { return m_inertiaCenter; }
glm::mat3 Solid::getInertiaTensor() const { return m_inertiaTensor; }

glm::vec3 Solid::getResultantForce() const { return m_resultantForce; }
glm::vec3 Solid::getSpeedVector() const { return m_speedVector; }
glm::vec3 Solid::getTorque() const { return m_torque; }
glm::vec3 Solid::getAngularMomentum() const { return m_angularMomentum; }
glm::vec3 Solid::getAngularSpeed() const { return glm::inverse(m_inertiaTensor) * m_angularMomentum; }

void Solid::calculateInertiaCenter() {
	m_inertiaCenter = glm::vec3(0);

	for (Mass mass : m_masses) {
		m_inertiaCenter += mass.getPosition() * mass.getMass();
	}

	m_inertiaCenter /= this->getTotalMass();
}

void Solid::calculateInertiaTensor() {
	m_inertiaTensor = glm::mat3(0);

	for (Mass mass : m_masses) {
		glm::vec3 v = mass.getPosition() - m_inertiaCenter;

		float A = v.y * v.y + v.z * v.z;
		float B = v.x * v.x + v.z * v.z;
		float C = v.x * v.x + v.y * v.y;
		float D = v.y * v.z;
		float E = v.x * v.z;
		float F = v.x * v.y;

		glm::mat3 massTensor(      //
		    glm::vec3(A, -F, -E),  //
		    glm::vec3(-F, B, -D),  //
		    glm::vec3(-E, -D, C)   //
		);

		m_inertiaTensor += mass.getMass() * massTensor;
	}
}

Solid& Solid::applyForce(Force const& force, UnitQuaternion rotation) {
	glm::mat2x3 wrench = this->getWrench(force, rotation);
	this->applyWrench(wrench);
	return *this;
}

glm::mat2x3 Solid::getWrench(Force const& force, UnitQuaternion rotation) const {
	glm::vec3 localReferential = force.getPosition() - m_inertiaCenter;
	glm::vec3 globalReferential = (rotation * localReferential * rotation.getConjugate()).getVector();
	return glm::mat2x3(glm::vec3(force.getDirection()), glm::vec3(glm::cross(globalReferential, force.getDirection())));
}

Solid& Solid::applyWrench(glm::mat2x3 wrench) {
	m_resultantForce += wrench[0];
	m_torque += wrench[1];
	return *this;
}

Solid& Solid::integrate(double deltaTime) {
	if (m_locked) {
		return *this;
	}

	m_speedVector += m_resultantForce * (float)deltaTime;
	m_angularMomentum += m_torque * (float)deltaTime;
	m_resultantForce = glm::vec3(0);
	m_torque = glm::vec3(0);
	return *this;
}

Solid::~Solid() {}



/* --- WORLDOBJECT --- */



WorldObject::WorldObject(vector<BoundingBox*> boundingBoxes, Solid& solid, Mesh& mesh)
    : m_boundingBoxes(boundingBoxes), m_solid(solid), m_mesh(mesh) {}

std::vector<BoundingBox*>& WorldObject::getBoundingBoxes() { return m_boundingBoxes; }
Mesh&                      WorldObject::getMesh() { return m_mesh; }
Solid&                     WorldObject::getSolid() { return m_solid; }

WorldObject& WorldObject::update(double deltaTime) {
	m_solid.integrate(deltaTime);
	m_mesh.translate(m_solid.getSpeedVector() * (float)deltaTime);
	glm::vec3 angularSpeed = m_solid.getAngularSpeed();
	float     norm = glm::length(angularSpeed);
	m_mesh.rotateSelf(norm * deltaTime / M_PI * 180, angularSpeed);
	return *this;
}

WorldObject::~WorldObject() {}



/* --- JOINT --- */



float Joint::springConstant = 100;
float Joint::dampingConstant = 20;

Joint::Joint(WorldObject* worldObject1, glm::vec3 wO1Contact, WorldObject* worldObject2, glm::vec3 wO2Contact)
    : m_worldObject1(worldObject1), m_wO1Contact(wO1Contact), m_worldObject2(worldObject2), m_wO2Contact(wO2Contact), m_twist() {}

WorldObject* Joint::getWorldObject1() { return m_worldObject1; }
WorldObject* Joint::getWorldObject2() { return m_worldObject2; }
glm::mat2x3& Joint::getTwist() { return m_twist; }

Joint::~Joint() {}



/* --- SKELETON --- */



Skeleton::Skeleton(vector<WorldObject*> worldObjects, vector<Joint*> joints) : m_worldObjects(worldObjects), m_joints(joints) {}

vector<Joint*>& Skeleton::getJoints() { return m_joints; }

void Skeleton::update(double deltaTime) {
	for (WorldObject* WorldObject : m_worldObjects) {
		WorldObject->update(deltaTime);
	}
	for (Joint* joint : m_joints) {
		joint->applyConstraints(deltaTime);
	}
}

Skeleton::~Skeleton() {}



/* --- PLANET --- */



Planet::Planet(float gravityIntensity)
    : m_clock(Clock()), m_gravityIntensity(gravityIntensity), m_scene(Scene()), m_skeletons(vector<Skeleton*>()) {}

Scene& Planet::getScene() { return m_scene; }

Planet& Planet::add(Skeleton* skeleton) {
	m_skeletons.push_back(skeleton);

	for (Joint* joint : skeleton->getJoints()) {
		m_scene.add(&joint->getWorldObject1()->getMesh());
		m_scene.add(&joint->getWorldObject2()->getMesh());
	}

	return *this;
}

Planet& Planet::remove(Skeleton* skeleton) {
	auto it = find(m_skeletons.begin(), m_skeletons.end(), skeleton);
	if (it != m_skeletons.end()) {
		for (Joint* joint : skeleton->getJoints()) {
			m_scene.remove(&joint->getWorldObject1()->getMesh());
			m_scene.remove(&joint->getWorldObject2()->getMesh());
		}
		m_skeletons.erase(it);
	}
	return *this;
}

void Planet::update() {
	m_clock.tick();

	for (Skeleton* skeleton : m_skeletons) {
		skeleton->update(m_clock.getDeltaTime());
	}
}

Planet::~Planet() {}



/* --- SPHEREBOUNDINGBOX --- */



SphereBoundingBox::SphereBoundingBox(glm::vec3 position, float radius, float restitutionCoef, float sliding)
    : BoundingBox::BoundingBox(restitutionCoef, sliding, position, BoundingBoxType::Sphere), m_radius(radius) {}

float SphereBoundingBox::getRadius() const { return m_radius; }

Force SphereBoundingBox::intersect(BoundingBox const& boundingBox, glm::vec3 hisTranslation, UnitQuaternion hisRotation,
                                   glm::vec3 thisTranslation, UnitQuaternion thisRotation, glm::vec3 thisSpeed) const {
	glm::vec3 thisPosition = m_position;
	thisPosition = (thisRotation * thisPosition * thisRotation.getConjugate()).getVector() + thisTranslation;
	glm::vec3 hisPosition = boundingBox.getPosition();
	hisPosition = (hisRotation * hisPosition * hisRotation.getConjugate()).getVector() + hisTranslation;

	if (boundingBox.getType() == BoundingBoxType::Sphere) {
		glm::vec3 thisToHis = hisPosition - thisPosition;
		float     distance = glm::length(thisToHis);
		float     intersectedPart = m_radius + boundingBox.getRadius() - distance;
		if (intersectedPart > 0) {
			Force reaction(glm::normalize(thisToHis) * m_radius,
			               glm::normalize(-thisToHis) * (float)glm::length(thisSpeed) * m_restitutionCoef);
			return reaction;
		}
	}
	return Force();
}

SphereBoundingBox::~SphereBoundingBox() {}



/* --- BALLJOINT --- */



BallJoint::BallJoint(WorldObject* worldObject1, glm::vec3 wO1Contact, WorldObject* worldObject2, glm::vec3 wO2Contact)
    : Joint::Joint(worldObject1, wO1Contact, worldObject2, wO2Contact) {
	m_twist = glm::mat2x3(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0));
}

void BallJoint::applyConstraints(double deltaTime) {
	UnitQuaternion rotation1 = m_worldObject1->getMesh().getRotation();
	UnitQuaternion rotation2 = m_worldObject2->getMesh().getRotation();

	float          mass1 = m_worldObject1->getSolid().getTotalMass();
	float          mass2 = m_worldObject2->getSolid().getTotalMass();
	float          totalMass = mass1 + mass2;
	float          t = mass2 / totalMass;
	UnitQuaternion averageRotation = rotation1.slerp(rotation1, rotation2, t);

	m_worldObject1->getMesh().rotateScene(averageRotation * rotation1.getConjugate());
	m_worldObject2->getMesh().rotateScene(averageRotation * rotation2.getConjugate());

	glm::vec3 point1 = m_worldObject1->getMesh().transform(m_wO1Contact);
	glm::vec3 point2 = m_worldObject2->getMesh().transform(m_wO2Contact);

	glm::vec3 middlePoint = (mass1 * point1 + mass2 * point2) / totalMass;

	m_worldObject1->getMesh().translate(middlePoint - point1);
	m_worldObject2->getMesh().translate(middlePoint - point2);
}


BallJoint::~BallJoint() {}
