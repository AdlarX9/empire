#include "utils.hpp"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;




/* --- QUATERNION --- */



Quaternion::Quaternion(double x, double y, double z, double w) : m_x(x), m_y(y), m_z(z), m_w(w) {}
Quaternion::Quaternion(glm::vec3& vector3) : m_w(0), m_x(vector3.x), m_y(vector3.y), m_z(vector3.z) {}

double Quaternion::x() const { return m_x; }
double Quaternion::y() const { return m_y; }
double Quaternion::z() const { return m_z; }
double Quaternion::w() const { return m_w; }

Quaternion& Quaternion::x(double x) {
	this->set(this->w(), x, this->y(), this->z());
	return *this;
}
Quaternion& Quaternion::y(double y) {
	this->set(this->w(), this->x(), y, this->z());
	return *this;
}
Quaternion& Quaternion::z(double z) {
	this->set(this->w(), this->x(), this->y(), z);
	return *this;
}
Quaternion& Quaternion::w(double w) {
	this->set(w, this->x(), this->y(), this->z());
	return *this;
}

double Quaternion::squaredLength() const { return m_w * m_w + m_x * m_x + m_y * m_y + m_z * m_z; }
double Quaternion::length() const { return sqrt(this->squaredLength()); }

Quaternion& Quaternion::normalize() {
	double n = this->length();
	if (n != 0) {
		m_x /= n;
		m_y /= n;
		m_z /= n;
		m_w /= n;
	}
	return *this;
}

Quaternion& Quaternion::set(double w, double x, double y, double z) {
	m_w = w;
	m_x = x;
	m_y = y;
	m_z = z;
	return *this;
}

void Quaternion::print(ostream& flow) const { flow << '(' << m_x << " ; " << m_y << " ; " << m_z << " ; " << m_w << ')'; }

Quaternion& Quaternion::conjugate() {
	m_x *= -1;
	m_y *= -1;
	m_z *= -1;
	return *this;
}

double Quaternion::dot(Quaternion const& quaternion) const {
	return m_w * quaternion.w() + m_x * quaternion.x() + m_y * quaternion.y() + m_z * quaternion.z();
}

glm::vec4 Quaternion::getValue() const { return glm::vec4(m_x, m_y, m_z, m_w); }
glm::vec3 Quaternion::getVector() const { return glm::vec3(m_x, m_y, m_z); }

glm::mat3 Quaternion::getMatrix() const {
	glm::quat q(m_w, m_x, m_y, m_z);
	glm::mat3 matrix = glm::toMat3(q);
	return matrix;
}


Quaternion& Quaternion::operator*=(Quaternion const& q) {
	double w(m_w), x(m_x), y(m_y), z(m_z), w1(q.w()), x1(q.x()), y1(q.y()), z1(q.z());
	m_x = w * x1 + x * w1 + y * z1 - z * y1;
	m_y = w * y1 + y * w1 - x * z1 + z * x1;
	m_z = w * z1 + z * w1 + x * y1 - y * x1;
	m_w = w * w1 - x * x1 - y * y1 - z * z1;
	return *this;
}
Quaternion& Quaternion::operator+=(Quaternion const& q) {
	m_w += q.w();
	m_x += q.x();
	m_y += q.y();
	m_z += q.z();
	return *this;
}
Quaternion& Quaternion::operator-=(Quaternion const& q) {
	m_w -= q.w();
	m_x -= q.x();
	m_y -= q.y();
	m_z -= q.z();
	return *this;
}
Quaternion& Quaternion::operator*=(double const& factor) {
	m_w *= factor;
	m_x *= factor;
	m_y *= factor;
	m_z *= factor;
	return *this;
}
Quaternion& Quaternion::operator/=(double const& divider) {
	if (divider != 0) {
		m_w /= divider;
		m_x /= divider;
		m_y /= divider;
		m_z /= divider;
	}
	return *this;
}

bool operator==(Quaternion const& q1, Quaternion const& q2) {
	return q1.w() == q2.w() and q1.x() == q2.x() and q1.y() == q2.y() and q1.z() == q2.z();
}
Quaternion operator*(Quaternion const& q1, Quaternion const& q2) {
	Quaternion result(q1);
	result *= q2;
	return result;
}
Quaternion operator+(Quaternion const& q1, Quaternion const& q2) {
	Quaternion result(q1);
	result += q2;
	return result;
}
Quaternion operator-(Quaternion const& q1, Quaternion const& q2) {
	Quaternion result(q1);
	result -= q2;
	return result;
}
Quaternion operator*(Quaternion const& q, double const& factor) {
	Quaternion result(q);
	return result *= factor;
}
Quaternion operator*(double const& factor, Quaternion const& q) {
	Quaternion result(q);
	return result *= factor;
}
Quaternion operator/(Quaternion const& q, double const& divider) {
	Quaternion result(q);
	return result /= divider;
}
Quaternion operator/(double const& divider, Quaternion const& q) {
	Quaternion result(q);
	return result /= divider;
}
ostream& operator<<(ostream& flow, Quaternion const& quaternion) {
	quaternion.print(flow);
	return flow;
}

Quaternion::~Quaternion() {}


/* --- UNITQUATERNION --- */


UnitQuaternion::UnitQuaternion(double angle, double x, double y, double z)
    : Quaternion::Quaternion(x * sin(angle * M_PI / 360.0f), y * sin(angle * M_PI / 360.0f), z * sin(angle * M_PI / 360.0f),
                             cos(angle * M_PI / 360.0f)) {
	this->normalize();
}

UnitQuaternion::UnitQuaternion(Quaternion q) : Quaternion::Quaternion(q.x(), q.y(), q.z(), q.w()) { this->normalize(); }
UnitQuaternion::UnitQuaternion(double angle, glm::vec3 vector3) : UnitQuaternion::UnitQuaternion(angle, vector3.x, vector3.y, vector3.z) {}

UnitQuaternion UnitQuaternion::getConjugate() const { return UnitQuaternion(Quaternion(-m_x, -m_y, -m_z, m_w)); }

UnitQuaternion& UnitQuaternion::set(double angle, double x, double y, double z) {
	double sin = std::sin(angle * M_PI / 360.0f);
	m_x = x * sin;
	m_y = y * sin;
	m_z = z * sin;
	m_w = cos(angle * M_PI / 360.0f);
	this->normalize();
	return *this;
}

UnitQuaternion& UnitQuaternion::set(double angle, glm::vec3 axis) { return this->set(angle, axis.x, axis.y, axis.z); }

float UnitQuaternion::getAngle() const {
	float angle = acos(m_w) * 2;
	return angle;
}

glm::vec3 UnitQuaternion::getAxis() const {
	float angle = this->getAngle();
	float sin = std::sin(angle);

	if (sin == 0) {
		return glm::vec3(0, 0, 1);
	}

	glm::vec3 axis = glm::vec3(m_x / sin, m_y / sin, m_z / sin);
	return axis;
}

UnitQuaternion UnitQuaternion::slerp(UnitQuaternion const& quaternion1, UnitQuaternion const& quaternion2, float t) const {
	UnitQuaternion q1 = quaternion1;
	UnitQuaternion q2 = quaternion2;

	float dot = q1.dot(q2);  // cosinus de l'angle
	if (dot < 0.0f) {        // pour prendre le plus court chemin sur la sphère
		q2 *= -1;
		dot *= -1;
	}

	if (dot > 0.999999999f) {
		return q1;
	}

	// Calcul de l'angle entre les deux quaternions
	float theta_0 = std::acos(dot);  // angle initial
	float theta = theta_0 * t;       // angle interpolé

	float sin_theta = std::sin(theta);
	float sin_theta_0 = std::sin(theta_0);

	float s1 = std::cos(theta) - dot * sin_theta / sin_theta_0;
	float s2 = sin_theta / sin_theta_0;

	return (q1 * s1 + q2 * s2).normalize();
}

glm::mat3 UnitQuaternion::rotate(glm::mat3 matrix) const {
	glm::mat3 rotationMatrix = this->getMatrix();
	glm::mat3 rotatedMatrix = rotationMatrix * matrix * glm::transpose(rotationMatrix);
	return rotatedMatrix;
}

glm::vec3 UnitQuaternion::rotate(glm::vec3 point) const { return (*this * point * this->getConjugate()).getVector(); }
glm::vec3 UnitQuaternion::invertRotate(glm::vec3 point) const { return this->getConjugate().rotate(point); }

UnitQuaternion::~UnitQuaternion() {}



/* --- MATRIX --- */


Matrix::Matrix(unsigned int n, unsigned int m) : m_n(n), m_m(m) {
	m_elements = new double[n * m];
	for (unsigned int i = 0; i < n * m; i++) {
		m_elements[i] = 0;
	}
}

Matrix::Matrix(unsigned int n, unsigned int m, int* elements) : m_n(n), m_m(m) {
	m_elements = new double[n * m];
	for (unsigned int i = 0; i < n * m; i++) {
		m_elements[i] = elements[i];
	}
}

unsigned int Matrix::n() const { return m_n; }
unsigned int Matrix::m() const { return m_m; }
double*      Matrix::elements() const { return m_elements; }

double Matrix::get(unsigned int i, unsigned int j) const {
	if (i < m_n && j < m_m) {
		return m_elements[i * m_m + j];
	}
	return 0;
}

Matrix& Matrix::set(unsigned int i, unsigned int j, int value) {
	if (i < m_n && j < m_m) {
		m_elements[i * m_m + j] = value;
	}
	return *this;
}

Matrix& Matrix::cwiseProduct(Matrix const& matrix) {
	if (m_n != matrix.n() || m_m != matrix.m()) {
		cerr << "Error: Matrix cwiseProduct is impossible with dimensions " << "(" << m_n << ", " << m_m << ") and (" << matrix.n() << ", "
		     << matrix.m() << ")." << endl;
		exit(EXIT_FAILURE);
	}
	for (unsigned int i = 0; i < m_n * m_m; i++) {
		m_elements[i] *= matrix.get(i / m_m, i % m_m);
	}
	return *this;
}

Matrix& Matrix::operator+=(Matrix const& matrix2) {
	if (m_n != matrix2.n() || m_m != matrix2.m()) {
		cerr << "Error: Matrix addition is impossible with dimensions " << "(" << m_n << ", " << m_m << ") and (" << matrix2.n() << ", "
		     << matrix2.m() << ")." << endl;
		exit(EXIT_FAILURE);
	}
	for (unsigned int i = 0; i < m_n * m_m; i++) {
		m_elements[i] += matrix2.get(i / m_m, i % m_m);
	}
	return *this;
}

Matrix& Matrix::operator-=(Matrix const& matrix2) {
	if (m_n != matrix2.n() || m_m != matrix2.m()) {
		cerr << "Error: Matrix subtraction is impossible with dimensions " << "(" << m_n << ", " << m_m << ") and (" << matrix2.n() << ", "
		     << matrix2.m() << ")." << endl;
		exit(EXIT_FAILURE);
	}
	for (unsigned int i = 0; i < m_n * m_m; i++) {
		m_elements[i] -= matrix2.get(i / m_m, i % m_m);
	}
	return *this;
}

Matrix& Matrix::operator*=(double const& factor) {
	for (unsigned int i = 0; i < m_n * m_m; i++) {
		m_elements[i] *= factor;
	}
	return *this;
}
Matrix& Matrix::operator/=(double const& divider) {
	if (divider != 0) {
		for (unsigned int i = 0; i < m_n * m_m; i++) {
			m_elements[i] /= divider;
		}
	}
	return *this;
}
Matrix& Matrix::operator*=(Matrix const& matrix2) {
	if (m_m != matrix2.n()) {
		cerr << "Error: Matrix multiplication is impossible with dimensions " << "(" << m_n << ", " << m_m << ") and (" << matrix2.n()
		     << ", " << matrix2.m() << ")." << endl;
		exit(EXIT_FAILURE);
	}
	Matrix result(m_n, matrix2.m());
	for (unsigned int i = 0; i < m_n; i++) {
		for (unsigned int j = 0; j < matrix2.m(); j++) {
			result.set(i, j, 0);
			for (unsigned int k = 0; k < m_m; k++) {
				result.set(i, j, result.get(i, j) + this->get(i, k) * matrix2.get(k, j));
			}
		}
	}
	delete[] m_elements;
	m_elements = result.elements();
	m_n = result.n();
	m_m = result.m();
	return *this;
}

Matrix operator*(Matrix const& matrix1, Matrix const& matrix2) {
	Matrix result(matrix1);
	return result *= matrix2;
}
Matrix operator+(Matrix const& matrix1, Matrix const& matrix2) {
	Matrix result(matrix1);
	return result += matrix2;
}
Matrix operator-(Matrix const& matrix1, Matrix const& matrix2) {
	Matrix result(matrix1);
	return result -= matrix2;
}
Matrix operator*(Matrix const& matrix, double const& factor) {
	Matrix result(matrix);
	return result *= factor;
}
Matrix operator/(Matrix const& matrix, double const& factor) {
	Matrix result(matrix);
	return result /= factor;
}
Matrix operator*(double const& factor, Matrix const& matrix) {
	Matrix result(matrix);
	return result *= factor;
}
Matrix operator/(double const& factor, Matrix const& matrix) {
	Matrix result(matrix);
	return result /= factor;
}

Matrix::~Matrix() { delete[] m_elements; }
