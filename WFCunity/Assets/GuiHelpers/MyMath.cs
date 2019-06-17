using System;
using UnityEngine;


public class MyMath
{
	/// <summary>
	/// A proper version of inverseLerp() that doesn't clamp between 0 and 1.
	/// WTF was Unity thinking
	/// </summary>
	public static float InverseLerp(float a, float b, float x)
	{
		return (x - a) / (b - a);
	}
}