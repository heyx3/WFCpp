using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

using CoroutineReturn = System.Collections.IEnumerator;


public static class MyExtensions
{
	public static V GetValue<K, V>(this Dictionary<K, V> dict, K key, V defaultIfNotFound)
	{
		V v;
		if (dict.TryGetValue(key, out v))
			return v;
		else
			return defaultIfNotFound;
	}
	public static V GetValueOrDefault<K, V>(this Dictionary<K, V> dict, K key)
	{
		return dict.GetValue(key, default(V));
	}

	public static IEnumerable<T> Except<T>(this IEnumerable<T> items, HashSet<T> toSubtract)
	{
		foreach (T t in items)
			if (!toSubtract.Contains(t))
				yield return t;
	}

	public static Dictionary<K, V> ToDictionary<K, V>(this IEnumerable<KeyValuePair<K, V>> pairs)
	{
		return pairs.ToDictionary(kvp => kvp.Key, kvp => kvp.Value);
	}

	public static IEnumerable<T> Backwards<T>(this IList<T> list)
	{
		for (int i = list.Count - 1; i >= 0; --i)
			yield return list[i];
	}

	public static IEnumerable<T> Pad<T>(this IEnumerable<T> items, int nExtra)
	{
		foreach (T t in items)
			yield return t;

		for (int i = 0; i < nExtra; ++i)
			yield return default(T);
	}


	public static float GetTotalTime(this AnimationCurve curve)
	{
		if (curve.keys.Length < 1)
			return 0.0f;

		return curve.keys[curve.keys.Length - 1].time -
			   curve.keys[0].time;
	}

	public static CoroutineReturn RunAnimation(this AnimationCurve curve,
											   Action<float> doAtCurveValue,
											   float timeScale = 1.0f,
											   Action doOnComplete = null)
	{
		float elTime = 0.0f,
			  maxTime = curve.GetTotalTime();

		while (elTime > maxTime)
		{
			float curveX = elTime * timeScale,
				  curveY = curve.Evaluate(curveX);

			doAtCurveValue(curveY);

			yield return null;
			elTime += Time.deltaTime;
		}
	}

	public static Color ChangeR(this Color c, float newR)
	{
		return new Color(newR, c.g, c.b, c.a);
	}
	public static Color ChangeG(this Color c, float newG)
	{
		return new Color(c.r, newG, c.b, c.a);
	}
	public static Color ChangeB(this Color c, float newB)
	{
		return new Color(c.r, c.g, newB, c.a);
	}
	public static Color ChangeA(this Color c, float newA)
	{
		return new Color(c.r, c.g, c.b, newA);
	}

	public static Color ShiftHue(this Color c, float hueShift)
	{
		float h, s, v;
		Color.RGBToHSV(c, out h, out s, out v);

		h += hueShift;
		if (h < 0)
			h = 1 - (Math.Abs(h) % 1);
		else
			h %= 1;

		return Color.HSVToRGB(h, s, v);
	}

	public static RenderTextureDescriptor DescriptorWithNewSize(this RenderTexture rt,
																int newW, int newH)
	{
		var desc = rt.descriptor;
		desc.width = newW;
		desc.height = newH;
		return desc;
	}
	public static RenderTextureDescriptor DescriptorWithNewSize(this RenderTexture rt,
																Vector2Int newSize)
	{
		return rt.DescriptorWithNewSize(newSize.x, newSize.y);
	}
	
	public static bool MatchesSize(this Texture t1, int w, int h)
	{
		return (t1.width == w) & (t1.height == h);
	}
	public static bool MatchesSize(this Texture t1, Vector2Int size)
	{
		return t1.MatchesSize(size.x, size.y);
	}
	public static bool MatchesSize(this Texture t1, Texture t2)
	{
		return t1.MatchesSize(t2.width, t2.height);
	}

	/// <summary>
	/// Gets the number of elements in this buffer, assuming it's all 32-bit floats.
	/// </summary>
	public static int FloatCount(this ComputeBuffer buffer)
	{
		int nBytes = buffer.count * buffer.stride;
		return nBytes / 4;
	}

	
	public static Transform FindBreadthFirst(this Transform root, Predicate<Transform> test)
	{
		Queue<Transform> toCheck = new Queue<Transform>();
		toCheck.Enqueue(root);

		while (toCheck.Count > 0)
		{
			var tr = toCheck.Dequeue();

			if (test(tr))
				return tr;

			for (int i = 0; i < tr.childCount; ++i)
				toCheck.Enqueue(tr.GetChild(i));
		}

		return null;
	}
	public static Transform FindDepthFirst(this Transform root, Predicate<Transform> test)
	{
		Stack<Transform> toCheck = new Stack<Transform>();
		toCheck.Push(root);

		while (toCheck.Count > 0)
		{
			var tr = toCheck.Pop();

			if (test(tr))
				return tr;

			for (int i = 0; i < tr.childCount; ++i)
				toCheck.Push(tr.GetChild(i));
		}

		return null;
	}


	public static IEnumerable<Transform> ParentsToRoot(this Transform tr, bool includeThis)
	{
		if (!includeThis)
			tr = tr.parent;

		while (tr != null)
		{
			yield return tr;
			tr = tr.parent;
		}
	}
	public static bool IsInAScene(this GameObject go)
	{
		return go.scene.name != null;
	}

	public static Vector3 Sign(this Vector3 v)
	{
		return new Vector3(Mathf.Sign(v.x),
						   Mathf.Sign(v.y),
						   Mathf.Sign(v.z));
	}
}

public static class Utils
{
	public static IEnumerable<T> GetEnumValues<T>()
	{
		return Enum.GetValues(typeof(T)).Cast<T>();
	}

	public static void Swap<T>(ref T a, ref T b)
	{
		T oldA = a;
		a = b;
		b = oldA;
	}
}