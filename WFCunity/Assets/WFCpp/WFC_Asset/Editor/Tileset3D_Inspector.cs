using UnityEngine;
using UnityEditor;

namespace WFC_CS.Editor
{
	[CustomEditor(typeof(Tileset3D))]
	public class Tileset3D_Inspector : UnityEditor.Editor
	{
		public override void OnInspectorGUI()
		{
			if (GUILayout.Button("Open Editor"))
				Tileset3D_EditorWindow.ShowWindow();
		}
	}
}