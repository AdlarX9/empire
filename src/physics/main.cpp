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

long long Clock::getDeltaTime() const { return m_deltaTime; }

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



BoundingBox::BoundingBox(float restitutionCoef, float sliding, BoundingBoxType type)
    : m_restitutionCoef(restitutionCoef), m_sliding(sliding), m_type(type) {}

BoundingBox::~BoundingBox() {}



/* --- SOLID --- */



Solid::Solid(vector<Mass> masses, bool locked)
    : m_masses(masses),
      m_inertiaCenter(glm::vec3(0)),
      m_inertiaTensor(glm::mat3(1)),
      m_speedVector(glm::vec3(0)),
      m_angularMomentum(UnitQuaternion()),
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

glm::vec3      Solid::getInertiaCenter() const { return m_inertiaCenter; }
glm::mat3      Solid::getInertiaTensor() const { return m_inertiaTensor; }
glm::vec3      Solid::getSpeedVector() const { return m_speedVector; }
UnitQuaternion Solid::getAngularMomentum() const { return m_angularMomentum; }

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

Solid& Solid::applyForce(Force const& force) {
	m_speedVector += force.getDirection() / this->getTotalMass();

	// à continuer avec les équations du moment cinétique
	glm::vec3 forceMomentum = glm::cross(force.getPosition() - m_inertiaCenter, force.getDirection());
	glm::vec3 angularVelocity = glm::inverse(m_inertiaTensor) * forceMomentum;
	float     angle = glm::length(angularVelocity);
	glm::vec3 axis = (angle > 0.0001f) ? glm::normalize(angularVelocity) : glm::vec3(0, 0, 1);  // fallback

	m_angularMomentum = UnitQuaternion(angle, axis) * m_angularMomentum;

	return *this;
}

Solid::~Solid() {}



/* --- WORLDOBJECT --- */



WorldObject::WorldObject(BoundingBox& boundingBox, Solid& solid, Mesh& mesh) : m_boundingBox(boundingBox), m_solid(solid), m_mesh(mesh) {}

Mesh& WorldObject::getMesh() { return m_mesh; }

WorldObject& WorldObject::update(long long deltaTime) {
	m_mesh.translate(m_solid.getSpeedVector() * (float)deltaTime);
	m_mesh.rotateScene(m_solid.getAngularMomentum().getAngle() * deltaTime, m_solid.getAngularMomentum().getAxis());
	return *this;
}

WorldObject::~WorldObject() {}



/* --- JOINT --- */



Joint::Joint(WorldObject* worldObject1, WorldObject* worldObject2) : m_worldObject1(worldObject1), m_worldObject2(worldObject2) {}

WorldObject* Joint::getWorldObject1() { return m_worldObject1; }
WorldObject* Joint::getWorldObject2() { return m_worldObject2; }

void Joint::update(long long deltaTime) {
	//
}

Joint::~Joint() {}



/* --- SKELETON --- */



Skeleton::Skeleton(vector<Joint*> joints) : m_joints(joints) {}

std::vector<Joint*>& Skeleton::getJoints() { return m_joints; }

void Skeleton::update(long long deltaTime) {
	for (Joint* joint : m_joints) {
		joint->update(deltaTime);
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
		m_scene.add(&joint->getWorldObject2()->getMesh());
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
