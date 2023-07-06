#pragma once
class EngineMath
{
public:
	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	static float RandF(float a, float b)
	{
		return a + rand() * (b - a);
	}

	static int Rand(int a, int b)
	{
		return a + rand() % ((b - a) + 1);
	}

	template<typename T>
	static T Min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}

	template<typename T>
	static T Max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}

	template<typename T>
	static T Lerp(const T& a, const T& b, float t)
	{
		return a + (b - a) * t;
	}

	template<typename T>
	static T Clamp(const T& x, const T& low, const T& high)
	{
		return x < low ? low : (x > high ? high : x);
	}

	static XMVECTOR SphericalToCartesian(float radius, float theta, float phi)
	{
		return XMVectorSet(radius * sinf(phi) * cosf(theta),
						   radius * cosf(phi),
						   radius * sinf(phi) * sinf(theta), 1.0f);
	}

	static XMMATRIX InverseTranspose(CXMMATRIX M)
	{
		XMMATRIX A = M;
		A.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

		XMVECTOR det = XMMatrixDeterminant(A);
		return XMMatrixTranspose(XMMatrixInverse(&det, A));
	}

	static XMFLOAT4X4 Identity4x4()
	{
		static XMFLOAT4X4 I(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);

		return I;
	}

	static void GetDistributionVectors(XMFLOAT4* vectors)
	{
		XMFLOAT4 vecs[14] = {};

		//	cube corners
		vecs[0]  = XMFLOAT4(+1.0f, +1.0f, +1.0f, 0.0f);
		vecs[1]  = XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);

		vecs[2]  = XMFLOAT4(-1.0f, +1.0f, +1.0f, 0.0f);
		vecs[3]  = XMFLOAT4(+1.0f, -1.0f, -1.0f, 0.0f);

		vecs[4]  = XMFLOAT4(+1.0f, +1.0f, -1.0f, 0.0f);
		vecs[5]  = XMFLOAT4(-1.0f, -1.0f, +1.0f, 0.0f);

		vecs[6]  = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
		vecs[7]  = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

		//	center of faces
		vecs[8]  = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
		vecs[9]  = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

		vecs[10] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
		vecs[11] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

		vecs[12] = XMFLOAT4(-1.0f, +1.0f, -1.0f, 0.0f);
		vecs[13] = XMFLOAT4(+1.0f, -1.0f, +1.0f, 0.0f);

		for (int i = 0; i < 14; ++i)
		{
			float s = RandF(0.25f, 1.0f);

			XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&vecs[i]));

			XMStoreFloat4(&vectors[i], v);
		}
	}

	static vector<float> GetGaussWeights(float sigma)
	{
		float powSigma = 2.0f * sigma * sigma;

		int blurRadius = (int)ceil(2.0f * sigma);
		assert(blurRadius <= 5); // MaxBlurRadius = 5

		vector<float> weights;
		weights.resize(2 * blurRadius + 1);

		float weightSum = 0.0f;

		for (int i = -blurRadius; i <= blurRadius; ++i)
		{
			float x = (float)i;
			weights[i + blurRadius] = expf(-x * x / powSigma);
			weightSum += weights[i + blurRadius];
		}

		for (int i = 0; i < weights.size(); ++i)
		{
			weights[i] /= weightSum;
		}

		return weights;
	}

	static float AngleFromXY(float x, float y);

	static XMVECTOR RandUnitVec3();
	static XMVECTOR RandHemisphereUnitVec3(XMVECTOR n);

	static float DegreeToRadian(float value);
	static XMFLOAT3 DegreesToRadians(float x, float y, float z);
	static XMFLOAT3 DegreesToRadians(const XMFLOAT3& degrees);
	static XMFLOAT3 DegreesToRadians(const XMVECTOR& degrees);

	static float RadianToDegree(float value);
	static XMFLOAT3 RadiansToDegrees(float x, float y, float z);
	static XMFLOAT3 RadiansToDegrees(const XMFLOAT3& radians);
	static XMFLOAT3 RadiansToDegrees(const XMVECTOR& radians);

	static const float Infinity;
	static const float Pi;
};

