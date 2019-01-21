typedef struct {
	float x, y;
} Vector2;

typedef struct {
	float x, y, z;
} Vector3;

typedef struct Transform Transform;
struct Transform {
	Transform* parent;
	Vector3 position;
	Vector3 rotation;
};

Transform* createTransform(Transform* parent);

Vector3* createVector3(float x, float y, float z);

Vector3 addVector3(Vector3* v1, Vector3* v2);
Vector3 minusVector3(Vector3* v1, Vector3* v2);
Vector3 multiplyVector3(Vector3* v1, float multiplier);
Vector3 lerpVector3(Vector3* startPos, Vector3* endPos, float time);
float magnitudeVector3(Vector3* vector);

Vector2 minusVector2(Vector2* v1, Vector2* v2);
float magnitudeVector2(Vector2* vector);

Vector3 TransformToForward(Transform* transform);
Vector3 TransformToRight(Transform* transform);
Vector3 TransformToUp(Transform* transform);

Vector3 NormalizeVector3(Vector3* vector);

Vector3 crossProductVector3(Vector3* v1, Vector3* v2);

void glTranslateVector3(Vector3* vector);
void glRotateVector3(Vector3* vector);
void glVertexVector3(Vector3* vector);
void lookAt(Vector3* eyes, Vector3* target, Vector3* up);
double myRandom(double min, double max);