using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;

namespace WFC_CS.Utils
{
	public static class MyGUI
	{
		public static void LabelFoldout(ref bool currentFoldout, ref Vector2 currentScroll,
										string label, float space,
										IEnumerable<string> entries,
										params GUILayoutOption[] scrollAreaLayout)
		{
			currentFoldout = EditorGUILayout.Foldout(currentFoldout, label);
			if (!currentFoldout)
				return;

			using (GUIBlock.Layout_Tab(space))
			{
				currentScroll = GUILayout.BeginScrollView(currentScroll, scrollAreaLayout);

				//Put "lines" (a.k.a. short boxes) between the entries.
				bool first = true;
				foreach (var entry in entries)
				{
					if (!first)
						GUILayout.Box("", GUILayout.Height(1.0f), GUILayout.ExpandWidth(true));
					first = false;

					GUILayout.Label(entry);
				}

				GUILayout.EndScrollView();
			}
		}

		public static void EditorTextArea(SerializedProperty prop, float labelWidth = -1.0f,
										  GUIContent label = null,
										  params GUILayoutOption[] layouts)
		{
			using (GUIBlock.Layout_Horizontal(layouts))
			{
				if (label == null)
					label = new GUIContent(prop.displayName);

				if (labelWidth > 0.0f)
					GUILayout.Label(label, GUILayout.Width(labelWidth));
				else
					GUILayout.Label(label);

				prop.stringValue = EditorGUILayout.TextArea(prop.stringValue);
			}
		}
	}
}
