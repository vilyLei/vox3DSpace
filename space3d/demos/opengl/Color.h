#pragma once

// L:[0, 100] a:[-128, 127] b:[-128, 127]
vec3 RGB2Lab(vec3 rgb)
{
	float R = rgb.x;
	float G = rgb.y;
	float B = rgb.z;

	if (R > 0.04045) { R = pow(((R + 0.055) / 1.055), 2.4); }
	else { R = R / 12.92; }

	if (G > 0.04045) { G = pow(((G + 0.055) / 1.055), 2.4); }
	else { G = G / 12.92; }

	if (B > 0.04045) { B = pow(((B + 0.055) / 1.055), 2.4); }
	else { B = B / 12.92; }

	float X = R * 0.412453 + G * 0.357580 + B * 0.180423;
	float Y = R * 0.212671 + G * 0.715160 + B * 0.072169;
	float Z = R * 0.019334 + G * 0.119193 + B * 0.950227;

	// Normalize for D65 white point
	X = X / 0.950456;
	Z = Z / 1.088754;

	// threshold
	float fX, fY, fZ;
	if (X > 0.008856) { fX = pow(X, 1.0 / 3.0); }
	else { fX = 7.787 * X + 16.0 / 116.0; }
	if (Y > 0.008856) { fY = pow(Y, 1.0 / 3.0); }
	else { fY = 7.787 * Y + 16.0 / 116.0; }
	if (Z > 0.008856) { fZ = pow(Z, 1.0 / 3.0); }
	else { fZ = 7.787 * Z + 16.0 / 116.0; }

	float L = (116.0 * fY) - 16.0;;
	float a = 500.0 * (fX - fY);
	float b = 200.0 * (fY - fZ);

	return vec3(L, a, b);
}

// R:[0, 1] G:[0, 1] B:[0, 1]
vec3 Lab2RGB(vec3 Lab)
{
	float Y = (Lab[0] + 16.) / 116.;
	float X = Lab[1] / 500. + Y;
	float Z = Y - Lab[2] / 200.;

	float X3 = X * X * X;
	if (X3 > 0.008856) { X = X3; }
	else { X = (X - 16. / 116.) / 7.787; }

	float Y3 = Y * Y * Y;
	if (Y3 > 0.008856) { Y = Y3; }
	else { Y = (Y - 16. / 116.) / 7.787; }

	float Z3 = Z * Z * Z;
	if (Z3 > 0.008856) { Z = Z3; }
	else { Z = (Z - 16. / 116.) / 7.787; }

	X = 0.950456 * X;
	Z = 1.088754 * Z;

	float R = X * 3.2406 + Y * -1.5372 + Z * -0.4986;
	float G = X * -0.9689 + Y * 1.8758 + Z * 0.0415;
	float B = X * 0.0557 + Y * -0.2040 + Z * 1.0570;

	if (R > 0.0031308) { R = 1.055 * pow(R, (1.0 / 2.4)) - 0.055; }
	else { R = 12.92 * R; }

	if (G > 0.0031308) { G = 1.055 * pow(G, (1.0 / 2.4)) - 0.055; }
	else { G = 12.92 * G; }

	if (B > 0.0031308) { B = 1.055 * pow(B, (1.0 / 2.4)) - 0.055; }
	else { B = 12.92 * B; }

	R = clamp(R, 0.0f, 1.0f);
	G = clamp(G, 0.0f, 1.0f);
	B = clamp(B, 0.0f, 1.0f);

	return vec3(R, G, B);
}

vec3 GrayColor(vec3 color)
{
	float gray = 0.299 * color.x + 0.587 * color.y + 0.114 * color.z;
	return vec3(gray, gray, gray);
}

vec3 blendColor(ENUM colorBlendMode, vec3 diffuseColor, vec3 texColor, vec3 meanColor)
{
	if (colorBlendMode == COLOR_BLEND_MODE_MULTIPLY)	// Multiply Mode
	{
		diffuseColor *= texColor;
	}
	else if (colorBlendMode == COLOR_BLEND_MODE_COLOR) // Color Mode
	{
		vec3 meanColorLab = RGB2Lab(GrayColor(meanColor));
		vec3 diffuseColorLab = RGB2Lab(diffuseColor);
		vec3 texColorLab = RGB2Lab(GrayColor(texColor));

		vec3 resultLab = texColorLab - meanColorLab + diffuseColorLab;
		diffuseColor = Lab2RGB(resultLab);
	}
	return diffuseColor;
}
