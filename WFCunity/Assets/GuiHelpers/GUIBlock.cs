﻿using System;
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