using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif


/// <summary>
/// An item that changes the GUI/GUILayout state for as long as it's alive.
/// </summary>
public class GUIBlock : IDisposable
{
	public static GUIBlock Color(Color newCol)
	{
		var old = GUI.color;
		GUI.color = newCol;
		return new GUIBlock(() => GUI.color = old);
	}
	public static GUIBlock BackgroundColor(Color newCol)
	{
		var old = GUI.backgroundColor;
		GUI.backgroundColor = newCol;
		return new GUIBlock(() => GUI.color = old);
	}
	public static GUIBlock ContentColor(Color newCol)
	{
		var old = GUI.contentColor;
		GUI.contentColor = newCol;
		return new GUIBlock(() => GUI.contentColor = old);
	}

	public static GUIBlock Enabled(bool enabled)
	{
		var old = GUI.enabled;
		GUI.enabled = enabled;
		return new GUIBlock(() => GUI.enabled = old);
	}

	public static GUIBlock Layout_Area(Rect area, GUIContent content = null)
	{
		if (content != null)
			GUILayout.BeginArea(area, content);
		else
			GUILayout.BeginArea(area);

		return new GUIBlock(() => GUILayout.EndArea());
	}
	public static GUIBlock Layout_Horizontal(params GUILayoutOption[] options)
	{
		GUILayout.BeginHorizontal(options);
		return new GUIBlock(GUILayout.EndHorizontal);
	}
	public static GUIBlock Layout_Vertical(params GUILayoutOption[] options)
	{
		GUILayout.BeginVertical(options);
		return new GUIBlock(GUILayout.EndVertical);
	}
	public static GUIBlock Layout_Scroll(ref Vector2 pos, params GUILayoutOption[] options)
	{
		pos = GUILayout.BeginScrollView(pos, options);
		return new GUIBlock(() => GUILayout.EndScrollView());
	}
	public static GUIBlock Layout_Tab(float spaceBefore = -1, float spaceAfter = 0)
	{
		GUILayout.BeginHorizontal();

		if (spaceBefore < 0)
			GUILayout.FlexibleSpace();
		else if (spaceBefore > 0)
			GUILayout.Space(spaceBefore);

		GUILayout.BeginVertical();

		return new GUIBlock(() =>
		{
			GUILayout.EndVertical();

			if (spaceAfter < 0)
				GUILayout.FlexibleSpace();
			else if (spaceAfter > 0)
				GUILayout.Space(spaceAfter);

			GUILayout.EndHorizontal();
		});
	}


	#if UNITY_EDITOR
	public static GUIBlock EditorLabelWidth(float newWidth)
	{
		var old = EditorGUIUtility.labelWidth;
		EditorGUIUtility.labelWidth = newWidth;
		return new GUIBlock(() => EditorGUIUtility.labelWidth = old);
	}

	public static GUIBlock Layout_Foldout(ref bool isOpen, string label,
									      float tabSize = 20, float tabEndSize = 0)
	{
		isOpen = EditorGUILayout.Foldout(isOpen, label);

		if (isOpen)
			return Layout_Tab(tabSize, tabEndSize);
		else
			return new GUIBlock(() => { });
	}

	public static GUIBlock ChangeCheck(Action onChanged)
	{
		EditorGUI.BeginChangeCheck();
		return new GUIBlock(() =>
		{
			if (EditorGUI.EndChangeCheck())
				onChanged();
		});
	}
	#endif


	private Action onFinished;
	public GUIBlock(Action _onFinished)
	{
		onFinished = _onFinished;
	}
	public void Dispose()
	{
		onFinished();
	}
}