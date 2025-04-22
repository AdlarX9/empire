#ifndef MATHS_UTILS
#define MATHS_UTILS

#include <glm/glm.hpp>
#include <iostream>

class Quaternion {
  protected:
	double m_w;
	double m_x;
	double m_y;
	double m_z;

  public:
	Quaternion(double x = 0, double y = 0, double z = 0, double w = 0);
	Quaternion(glm::vec3& vector3);

	Quaternion& x(double x);
	double      x() const;
	Quaternion& y(double y);
	double      y() const;
	Quaternion& z(double z);
	double      z() const;
	Quaternion& w(double w);
	double      w() const;

	double              squaredLength() const;
	double              length() const;
	virtual Quaternion& set(double w = 0, double x = 0, double y = 0, double z = 0);
	Quaternion&         normalize();
	Quaternion&         conjugate();
	Quaternion          getConjugate() const;
	double              dot(Quaternion const& quaternion) const;
	glm::vec4           getValue() const;
	glm::vec3           getVector() const;
	glm::mat3           getMatrix() const;

	void print(std::ostream& flow) const;

	~Quaternion();

	Quaternion& operator*=(Quaternion const& quaternion);
	Quaternion& operator+=(Quaternion const& quaternion);
	Quaternion& operator-=(Quaternion const& quaternion);
	Quaternion& operator*=(double const& factor);
	Quaternion& operator/=(double const& factor);
};

Quaternion    operator*(Quaternion const& quaternion1, Quaternion const& quaternion2);
Quaternion    operator+(Quaternion const& quaternion1, Quaternion const& quaternion2);
Quaternion    operator-(Quaternion const& quaternion1, Quaternion const& quaternion2);
Quaternion    operator*(Quaternion const& quaternion, double const& factor);
Quaternion    operator/(Quaternion const& quaternion, double const& divider);
Quaternion    operator*(double const& factor, Quaternion const& quaternion);
Quaternion    operator/(double const& divider, Quaternion const& quaternion);
bool          operator==(Quaternion const& quaternion1, Quaternion const& quaternion2);
std::ostream& operator<<(std::ostream& flow, Quaternion const& quaternion);



class UnitQuaternion : public Quaternion {
  public:
	UnitQuaternion(double angle = 0, double x = 0, double y = 0, double z = 0);
	UnitQuaternion(double angle, glm::vec3 vector3);
	UnitQuaternion(Quaternion quaternion);
	UnitQuaternion& set(double angle, double x = 0, double y = 0, double z = 0);
	UnitQuaternion& set(double angle, glm::vec3 axis);
	glm::vec3       getAxis() const;
	float           getAngle() const;
	UnitQuaternion  slerp(UnitQuaternion const& quaternion1, UnitQuaternion const& quaternion2, float t) const;
	glm::vec3       rotate(glm::vec3 point) const;
	~UnitQuaternion();
};



class Matrix {
  private:
	unsigned int m_n;
	unsigned int m_m;
	double*      m_elements;

  public:
	Matrix(unsigned int n, unsigned int m);
	Matrix(unsigned int n, unsigned int m, int* elements);

	unsigned int n() const;
	unsigned int m() const;
	double*      elements() const;
	double       get(unsigned int i, unsigned int j) const;
	Matrix&      set(unsigned int i, unsigned int j, int value);
	Matrix&      cwiseProduct(Matrix const& matrix2);

	~Matrix();

	Matrix& operator*=(Matrix const& matrix2);
	Matrix& operator+=(Matrix const& matrix2);
	Matrix& operator-=(Matrix const& matrix2);
	Matrix& operator*=(double const& factor);
	Matrix& operator/=(double const& factor);
};

Matrix operator*(Matrix const& matrix1, Matrix const& matrix2);
Matrix operator+(Matrix const& matrix1, Matrix const& matrix2);
Matrix operator-(Matrix const& matrix1, Matrix const& matrix2);
Matrix operator*(Matrix const& matrix, double const& factor);
Matrix operator/(Matrix const& matrix, double const& factor);
Matrix operator*(double const& factor, Matrix const& matrix);
Matrix operator/(double const& factor, Matrix const& matrix);

#endif
