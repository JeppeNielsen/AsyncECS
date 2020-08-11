#include "Matrix3x3.hpp"
#include <cmath>

using namespace Game;

Matrix3x3 Matrix3x3::CreateIdentity() {
	Matrix3x3 mat;
	mat.Identity();
	return mat;
}

Matrix3x3 Matrix3x3::CreateRotation(float angle) {
	Matrix3x3 mat;
	mat.Rotation(angle);
	return mat;
}

Matrix3x3 Matrix3x3::CreateScale(const Vector2& scale) {
	Matrix3x3 mat;
	mat.Scale(scale);
	return mat;
}

Matrix3x3 Matrix3x3::CreateTranslation(const Vector2& position) {
	Matrix3x3 mat;
	mat.Translation(position);
	return mat;
}

void Matrix3x3::Scale(const Vector2& scale) {
	m[1][0] = 0;
	m[2][0] = 0;
	m[0][1] = 0;
	m[2][1] = 0;
	m[0][2] = 0;
	m[1][2] = 0;
	m[2][2] = 1.0f;
	m[0][0] = scale.x;
	m[1][1] = scale.y;
}

void Matrix3x3::Rotation(float angle) {
	float Sin = sinf(angle);
	float Cos = cosf(angle);
	m[0][0] = Cos; m[0][1] = Sin; m[0][2] = 0;
	m[1][0] = -Sin; m[1][1] = Cos; m[1][2] = 0;
	m[2][0] = 0; m[2][1] = 0; m[2][2] = 1;
}

void Matrix3x3::Translation(const Vector2& translation) {
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
	m[2][0] = translation.x; m[2][1] = translation.y; m[2][2] = 1;
}

void Matrix3x3::TRS(const Vector2& translation, float angle, const Vector2& scale) {
    float Sin = sinf(angle) * scale.x;
    float Cos = cosf(angle) * scale.y;
    m[0][0] = Cos; m[0][1] = Sin; m[0][2] = 0;
    m[1][0] = -Sin; m[1][1] = Cos; m[1][2] = 0;
    m[2][0] = translation.x; m[2][1] = translation.y; m[2][2] = 1;
}

Vector2 Matrix3x3::Position() const {
	return Vector2(m[2][0], m[2][1]);
}

float Matrix3x3::Angle() const {
	return atan2(m[1][0], m[0][0]);
}

Matrix3x3::Matrix3x3() { }

Matrix3x3::~Matrix3x3() { }

Matrix3x3::Matrix3x3(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
	m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
	m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
	m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
}

Matrix3x3::Matrix3x3(const Matrix3x3& mat) {
	m[0][0] = mat.m[0][0];	m[0][1] = mat.m[0][1];	m[0][2] = mat.m[0][2];
	m[1][0] = mat.m[1][0];	m[1][1] = mat.m[1][1];	m[1][2] = mat.m[1][2];
	m[2][0] = mat.m[2][0];	m[2][1] = mat.m[2][1];	m[2][2] = mat.m[2][2];
}

void Matrix3x3::Set(float m00, float m01, float m02, float m10, float m11, float m12, float m20, float m21, float m22) {
	m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
	m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
	m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
}


void Matrix3x3::Identity() {  
	m[1][0] = 0;
	m[2][0] = 0;
	m[0][1] = 0;
	m[2][1] = 0;
	m[0][2] = 0;
	m[1][2] = 0;
	m[0][0] = m[1][1] = m[2][2] = 1.0f; 		
}

void Matrix3x3::GetRow(const int r, float& x, float& y, float& z) const	{ 
	x = m[r][0];	
	y = m[r][1];	
	z = m[r][2];	
}

void Matrix3x3::Mult(const Matrix3x3& a, const Matrix3x3& b) {
	m[0][0] = a.m[0][0] * b.m[0][0] + a.m[0][1] * b.m[1][0] + a.m[0][2] * b.m[2][0];
	m[0][1] = a.m[0][0] * b.m[0][1] + a.m[0][1] * b.m[1][1] + a.m[0][2] * b.m[2][1];
	m[0][2] = a.m[0][0] * b.m[0][2] + a.m[0][1] * b.m[1][2] + a.m[0][2] * b.m[2][2];
	m[1][0] = a.m[1][0] * b.m[0][0] + a.m[1][1] * b.m[1][0] + a.m[1][2] * b.m[2][0];
	m[1][1] = a.m[1][0] * b.m[0][1] + a.m[1][1] * b.m[1][1] + a.m[1][2] * b.m[2][1];
	m[1][2] = a.m[1][0] * b.m[0][2] + a.m[1][1] * b.m[1][2] + a.m[1][2] * b.m[2][2];
	m[2][0] = a.m[2][0] * b.m[0][0] + a.m[2][1] * b.m[1][0] + a.m[2][2] * b.m[2][0];
	m[2][1] = a.m[2][0] * b.m[0][1] + a.m[2][1] * b.m[1][1] + a.m[2][2] * b.m[2][1];
	m[2][2] = a.m[2][0] * b.m[0][2] + a.m[2][1] * b.m[1][2] + a.m[2][2] * b.m[2][2];
}

float Matrix3x3::Determinant() const {
	return (m[0][0]*m[1][1]*m[2][2] + m[0][1]*m[1][2]*m[2][0] + m[0][2]*m[1][0]*m[2][1])
		-  (m[2][0]*m[1][1]*m[0][2] + m[2][1]*m[1][2]*m[0][0] + m[2][2]*m[1][0]*m[0][1]);
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& mat) const {
	return Matrix3x3(
	m[0][0]*mat.m[0][0] + m[0][1]*mat.m[1][0] + m[0][2]*mat.m[2][0],
	m[0][0]*mat.m[0][1] + m[0][1]*mat.m[1][1] + m[0][2]*mat.m[2][1],
	m[0][0]*mat.m[0][2] + m[0][1]*mat.m[1][2] + m[0][2]*mat.m[2][2],

	m[1][0]*mat.m[0][0] + m[1][1]*mat.m[1][0] + m[1][2]*mat.m[2][0],
	m[1][0]*mat.m[0][1] + m[1][1]*mat.m[1][1] + m[1][2]*mat.m[2][1],
	m[1][0]*mat.m[0][2] + m[1][1]*mat.m[1][2] + m[1][2]*mat.m[2][2],

	m[2][0]*mat.m[0][0] + m[2][1]*mat.m[1][0] + m[2][2]*mat.m[2][0],
	m[2][0]*mat.m[0][1] + m[2][1]*mat.m[1][1] + m[2][2]*mat.m[2][1],
	m[2][0]*mat.m[0][2] + m[2][1]*mat.m[1][2] + m[2][2]*mat.m[2][2]);
}

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& mat) {
	float x,y,z;

	GetRow(0, x,y,z);
	m[0][0] = x*mat.m[0][0] + y*mat.m[1][0] + z*mat.m[2][0];
	m[0][1] = x*mat.m[0][1] + y*mat.m[1][1] + z*mat.m[2][1];
	m[0][2] = x*mat.m[0][2] + y*mat.m[1][2] + z*mat.m[2][2];

	GetRow(1, x,y,z);
	m[1][0] = x*mat.m[0][0] + y*mat.m[1][0] + z*mat.m[2][0];
	m[1][1] = x*mat.m[0][1] + y*mat.m[1][1] + z*mat.m[2][1];
	m[1][2] = x*mat.m[0][2] + y*mat.m[1][2] + z*mat.m[2][2];

	GetRow(2, x,y,z);
	m[2][0] = x*mat.m[0][0] + y*mat.m[1][0] + z*mat.m[2][0];
	m[2][1] = x*mat.m[0][1] + y*mat.m[1][1] + z*mat.m[2][1];
	m[2][2] = x*mat.m[0][2] + y*mat.m[1][2] + z*mat.m[2][2];
	return	*this;
}

Matrix3x3 Matrix3x3::Invert() {
	float Det = Determinant();	// Must be !=0
	float OneOverDet = 1.0f / Det;

	Matrix3x3 temp;
	temp.m[0][0] = +(m[1][1] * m[2][2] - m[2][1] * m[1][2]) * OneOverDet;
	temp.m[1][0] = -(m[1][0] * m[2][2] - m[2][0] * m[1][2]) * OneOverDet;
	temp.m[2][0] = +(m[1][0] * m[2][1] - m[2][0] * m[1][1]) * OneOverDet;
	temp.m[0][1] = -(m[0][1] * m[2][2] - m[2][1] * m[0][2]) * OneOverDet;
	temp.m[1][1] = +(m[0][0] * m[2][2] - m[2][0] * m[0][2]) * OneOverDet;
	temp.m[2][1] = -(m[0][0] * m[2][1] - m[2][0] * m[0][1]) * OneOverDet;
	temp.m[0][2] = +(m[0][1] * m[1][2] - m[1][1] * m[0][2]) * OneOverDet;
	temp.m[1][2] = -(m[0][0] * m[1][2] - m[1][0] * m[0][2]) * OneOverDet;
	temp.m[2][2] = +(m[0][0] * m[1][1] - m[1][0] * m[0][1]) * OneOverDet;

	return	temp;
}

Vector2 Matrix3x3::TransformPoint(const Vector2& source) const {
	return Vector2(
        m[2][0] + source.x * m[0][0] + source.y * m[1][0],
        m[2][1] + source.x * m[0][1] + source.y * m[1][1]
    );
}

Vector2 Matrix3x3::TransformVector(const Vector2& source) const {
	return Vector2(
        source.x * m[0][0] + source.y * m[1][0],
        source.x * m[0][1] + source.y * m[1][1]
    );
}

void Matrix3x3::TransformPointRef(Vector2& source) const {
	float temp = source.x;
	source.x = m[2][0] + temp * m[0][0] + source.y * m[1][0];
	source.y = m[2][1] + temp * m[0][1] + source.y * m[1][1];
}

void Matrix3x3::TransformPointRef2(Vector2& dest, const Vector2& source) const {
	dest.x = m[2][0] + source.x * m[0][0] + source.y * m[1][0];
	dest.y = m[2][1] + source.x * m[0][1] + source.y * m[1][1];
}

void Matrix3x3::TransformVectorRef(Vector2& source) const {
	float temp = source.x;
	source.x = source.x * m[0][0] + source.y * m[1][0];
	source.y = temp * m[0][1] + source.y * m[1][1];
}

bool Matrix3x3::operator ==(const Matrix3x3 &other) {
	return !(
		m[0][0]!=other.m[0][0] || 
		m[0][1]!=other.m[0][1] ||
		m[0][2]!=other.m[0][2] ||
		m[1][0]!=other.m[1][0] ||
		m[1][1]!=other.m[1][1] ||
		m[1][2]!=other.m[1][2] ||
		m[2][0]!=other.m[2][0] ||
		m[2][1]!=other.m[2][1] ||
		m[2][2]!=other.m[2][2]);
}

bool Matrix3x3::operator !=(const Matrix3x3 &other) {
	return (
		m[0][0]!=other.m[0][0] || 
		m[0][1]!=other.m[0][1] ||
		m[0][2]!=other.m[0][2] ||
		m[1][0]!=other.m[1][0] ||
		m[1][1]!=other.m[1][1] ||
		m[1][2]!=other.m[1][2] ||
		m[2][0]!=other.m[2][0] ||
		m[2][1]!=other.m[2][1] ||
		m[2][2]!=other.m[2][2]);
}

void Matrix3x3::GetGlMatrix(float* mat) {
	//float* mat = new float[16];
	/*
	m[0][0] = 1; m[0][1] = 0; m[0][2] = 0;
	m[1][0] = 0; m[1][1] = 1; m[1][2] = 0;
	m[2][0] = v.x; m[2][1] = v.y; m[2][2] = 1;
	*/

	

	mat[0] = m[0][0]; mat[1] = m[0][1]; mat[2] = m[0][2]; mat[3] = 0;
	mat[4] = m[1][0]; mat[5] = m[1][1]; mat[6] = m[1][2]; mat[7] = 0;
	mat[8] = 0; mat[9] = 0; mat[10] = 1; mat[11] = 0;
	mat[12] = m[2][0]; mat[13] = m[2][1]; mat[14] = 0; mat[15] = 1;
}

