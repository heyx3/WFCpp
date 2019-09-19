using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif

namespace WFC_CS.Utils
{
	#region GUI Layout: Horizontal
	public class GUILayoutHorizontal : GUI.Scope
	{
		public GUILayoutHorizontal(bool box = false)
		{
			if (box)
				GUILayout.BeginHorizontal("box");
			else
				GUILayout.BeginHorizontal();
		}
		protected override void CloseScope()
		{
			GUILayout.EndHorizontal();
		}
	}
	#endregion
	#region GUI Layout: Vertical
	public class GUILayoutVertical : GUI.Scope
	{
		public GUILayoutVertical(bool box = false)
		{
			if (box)
				GUILayout.BeginVertical("box");
			else
				GUILayout.BeginVertical();
		}
		protected override void CloseScope()
		{
			GUILayout.EndVertical();
		}
	}
	#endregion
	#region GUI Layout: Compact
	public class GUILayoutCompact : GUI.Scope
	{
		public GUILayoutCompact(float preSpace = 0.0f, float postSpace = -1.0f)
		{
			GUILayout.BeginHorizontal();
			Space(preSpace);
			_postSpace = postSpace;
		}
		protected override void CloseScope()
		{
			Space(_postSpace);
			GUILayout.EndHorizontal();
		}

		private float _postSpace;
		private void Space(float space)
		{
			if (space < 0)
				GUILayout.FlexibleSpace();
			else if (space > 0)
				GUILayout.Space(space);
		}
	}
	#endregion
	#region GUI Layout: Tab
	public class GUILayoutTab : GUILayoutCompact
	{
		public GUILayoutTab(float tabInSpace = 25.0f, float tabEndSpace = 0)
			: base(tabInSpace, tabEndSpace)
		{
			GUILayout.BeginVertical();
		}
		protected override void CloseScope()
		{
			GUILayout.EndVertical();
			base.CloseScope();
		}
	}
	#endregion

	#if UNITY_EDITOR
	#region Editor GUI: Editor Label Width
	public class EditorGUILabelWidth : GUI.Scope
	{
		public float OriginalWidth;
		public EditorGUILabelWidth(float newWidth)
		{
			OriginalWidth = EditorGUIUtility.labelWidth;
			EditorGUIUtility.labelWidth = newWidth;
		}
		protected override void CloseScope()
		{
			EditorGUIUtility.labelWidth = OriginalWidth;
		}
	}
	#endregion
	#region Editor GUI: Field Width
	public class EditorGUIFieldWidth : GUI.Scope
	{
		public float OriginalWidth;
		public EditorGUIFieldWidth(float newWidth)
		{
			OriginalWidth = EditorGUIUtility.fieldWidth;
			EditorGUIUtility.fieldWidth = newWidth;
		}
		protected override void CloseScope()
		{
			EditorGUIUtility.fieldWidth = OriginalWidth;
		}
	}
	#endregion
	#endif
}