#include "utils.hpp"

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>

using namespace std;




/* --- QUATERNION --- */



Quaternion::Quaternion(double w, double x, double y, double z) : m_w(w), m_x(x), m_y(y), m_z(z) {}
Quaternion::Quaternion(sf::Vector3<double>& vector3) : m_w(0), m_x(vector3.x), m_y(vector3.y), m_z(vector3.z) {}
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

Quaternion  Quaternion::getConjugate() const { return Quaternion(-m_x, -m_y, -m_z, m_w); }
Quaternion& Quaternion::conjugate() {
	m_x *= -1;
	m_y *= -1;
	m_z *= -1;
	return *this;
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
    : Quaternion::Quaternion(x * sin(angle / 360 * M_PI), y * sin(angle / 360 * M_PI), z * sin(angle / 360 * M_PI),
                             cos(angle / 360 * M_PI)) {
	this->normalize();
}

UnitQuaternion::UnitQuaternion(Quaternion q) : Quaternion::Quaternion(q.x(), q.y(), q.z(), q.w()) { this->normalize(); }

UnitQuaternion& UnitQuaternion::set(double angle, double x, double y, double z) {
	double sin = std::sin(angle / 360 * M_PI);
	m_x = x * sin;
	m_y = y * sin;
	m_z = z * sin;
	m_w = cos(angle / 360 * M_PI);
	this->normalize();
	return *this;
}

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
