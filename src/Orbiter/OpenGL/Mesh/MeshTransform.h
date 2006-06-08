#ifndef MeshTransform_H_
#define MeshTransform_H_

class MeshTransform
{
public:
	/**
	*	Linear 3D transform matrix (4 x 4 size)
	*/
	float TransformMatrix[16];

	MeshTransform(void);
	virtual ~MeshTransform(void);
	void SetIdentity();
	
	void Scale(float X, float Y, float Z);
	void ApplyScale(float X, float Y, float Z);
	
	void Translate(float X, float Y, float Z);
	void ApplyTranslate(float X, float Y, float Z);

	void RotateX(float Angle);
	void ApplyRotateX(float Angle);
	void RotateY(float Angle);
	void ApplyRotateY(float Angle);
	void RotateZ(float Angle);
	void ApplyRotateZ(float Angle);
	void ApplyTransform(MeshTransform& Transform);

	void CopyFrom(MeshTransform& Transform);
};

#endif
