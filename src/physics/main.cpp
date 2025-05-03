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
      m_speedVector(glm::vec3(0)),
      m_angularMomentum(glm::vec3(0)),
      m_locked(locked) {
	this->calculateInertiaCenter();
	this->calculateInertiaTensor();
}

bool Solid::getLocked() const { return m_locked; }

float Solid::getTotalMass() const {
	float totalMass(0);

	for (Mass mass : m_masses) {
		totalMass += mass.getMass();
	}

	return totalMass;
}

glm::vec3 Solid::getInertiaCenter() const { return m_inertiaCenter; }
glm::mat3 Solid::getInertiaTensor() const { return m_inertiaTensor; }

glm::vec3 Solid::getSpeedVector() const { return m_speedVector; }
glm::vec3 Solid::getAngularMomentum() const { return m_angularMomentum; }

glm::vec3 Solid::getAngularSpeed() const {
	glm::vec3 angularSpeed = glm::inverse(m_inertiaTensor) * m_angularMomentum;  // L = I . w <=> w = I-1 . L
	return angularSpeed;
}

glm::vec3 Solid::getSpeedAt(glm::vec3 point) const { return m_speedVector + glm::cross(this->getAngularSpeed(), point); }

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

		m_inertiaTensor += mass.getMass() * massTensor;  // I = ∑(Ii . mi)
	}
}


Solid& Solid::setSpeedVector(glm::vec3 speedVector) {
	m_speedVector = speedVector;
	return *this;
}

Solid& Solid::setAngularMomentum(glm::vec3 angularMomentum) {
	m_angularMomentum = angularMomentum;
	return *this;
}

Solid& Solid::incrementSpeed(glm::vec3 deltaSpeed) {
	m_speedVector += deltaSpeed;
	return *this;
}

Solid& Solid::incrementAngularMomentum(glm::vec3 deltaAngularMomentum) {
	m_angularMomentum += deltaAngularMomentum;
	return *this;
}

Solid& Solid::applyLinearImpulse(glm::vec3 impulse) {
	m_speedVector += impulse;
	return *this;
}

Solid& Solid::applyAngularImpulse(glm::vec3 impulse) {
	m_angularMomentum += impulse;
	return *this;
}

Solid::~Solid() {}



/* --- WORLDOBJECT --- */



WorldObject::WorldObject(std::vector<BoundingBox*> boundingBoxes, Solid& solid, Mesh& mesh)
    : m_boundingBoxes(boundingBoxes), m_solid(solid), m_resultantForce(glm::vec3(0)), m_torque(glm::vec3(0)), m_mesh(mesh) {}

glm::vec3 WorldObject::getResultantForce() const { return m_resultantForce; }
glm::vec3 WorldObject::getTorque() const { return m_torque; }

WorldObject& WorldObject::applyForce(Force const& force) {
	this->applyWrench(glm::mat2x3(force.getDirection(), glm::vec3(0)), force.getPosition());
	return *this;
}

glm::mat2x3 WorldObject::getWrench(Force const& force) const {
	// Moment M = OA ^ F
	// Torseur de type [Tx Ty Tz Mx My Mz] dans le repère monde
	return glm::mat2x3(
	    glm::vec3(force.getDirection()),
	    glm::vec3(glm::cross(m_mesh.getRotation().rotate(force.getPosition() - m_solid.getInertiaCenter()), force.getDirection())));
}

WorldObject& WorldObject::applyWrench(glm::mat2x3 wrench, glm::vec3 point) {
	m_resultantForce += wrench[0];                                                                                   // Fo = Fa
	m_torque += wrench[1] + glm::cross(m_mesh.getRotation().rotate(point - m_solid.getInertiaCenter()), wrench[0]);  // Mo = Ma + OA ^ F
	return *this;
}

std::vector<BoundingBox*>& WorldObject::getBoundingBoxes() { return m_boundingBoxes; }
Mesh&                      WorldObject::getMesh() { return m_mesh; }
Solid&                     WorldObject::getSolid() { return m_solid; }

// Script de mise à jour de la physique
WorldObject& WorldObject::update(double deltaTime) {
	if (m_solid.getLocked()) {
		return *this;
	}

	// On applique la somme des forces
	glm::vec3 worldDeltaSpeed = m_resultantForce / m_solid.getTotalMass() * (float)deltaTime;  // a = F / m et dv / dt = a
	m_solid.incrementSpeed(worldDeltaSpeed);

	// On applique la somme des moments
	glm::vec3 worldDeltaAngularMomentum = m_torque * (float)deltaTime;                               // dL / dt = ∑M
	m_solid.incrementAngularMomentum(m_mesh.getRotation().invertRotate(worldDeltaAngularMomentum));  // on bascule dans le repère local

	// On remet à 0 les forces et les moments
	m_resultantForce = glm::vec3(0);
	m_torque = glm::vec3(0);

	// Translation linéaire
	m_mesh.translate(m_solid.getSpeedVector() * (float)deltaTime);  // dx/dt = v

	// Rotation autour de l'axe instantané
	glm::vec3 angularSpeed = m_solid.getAngularSpeed();  // dans le repère local
	float     norm = glm::length(angularSpeed);          // en rad/s

	if (norm > 1e-9f) {  // éviter les divisions par 0 et les petites rotations inutiles
		glm::vec3 axis = glm::normalize(angularSpeed);
		float     angle = norm * (float)deltaTime / M_PI * 180.0f;           // conversion rad → degré
		m_mesh.rotateSelf(angle, angularSpeed, m_solid.getInertiaCenter());  // rotation dans le repère local
	}

	return *this;
}

WorldObject::~WorldObject() {}



/* --- JOINT --- */



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
	if (deltaTime <= 0) {
		return;
	}
}

BallJoint::~BallJoint() {}
