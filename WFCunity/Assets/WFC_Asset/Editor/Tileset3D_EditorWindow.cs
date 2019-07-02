using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;
using UnityEngine.Assertions;

namespace WFC_CS.Editor
{
	public class Tileset3D_EditorWindow : EditorWindow
	{
		[MenuItem("Assets/Tileset3D Editor")]
		public static void ShowWindow()
		{
			GetWindow<Tileset3D_EditorWindow>().Show();
		}
		[UnityEditor.Callbacks.DidReloadScripts]
		public static void CloseWindows()
		{
			//Close all open windows, because hot reloading breaks everything.

			var windows = Resources.FindObjectsOfTypeAll<Tileset3D_EditorWindow>();
			if (windows.Length < 1)
				return;
			 
			Debug.LogWarning("Closing all Tileset3D editor windows due to recompilation." +
							 " Any unsaved work is already lost. There are expected to be " +
							 windows.Length + " exceptions directly below this message.");
			foreach (var window in windows)
				window.Close();
		}


		private Tileset3D_Header editorPane;


		private void Awake()
		{
			titleContent = new GUIContent("Tileset3D");
			minSize = new Vector2(710, 600);

			//If a Tileset3D is already selected, load that one automatically.
			var selectedTilesets = Selection.GetFiltered<Tileset3D>(SelectionMode.Assets);
			var tileset = (selectedTilesets.Length == 1) ? selectedTilesets[0] : null;

			//We're always working with copies of the original asset,
			//    so no harm in throwing exceptions.
			Assert.raiseExceptions = true;

			//Set up the UI panels.
			editorPane = (tileset == null) ?
						     new Tileset3D_Header() :
							 new Tileset3D_Header(tileset);
			editorPane.OwnerWindow = this;
		}
		private void OnDestroy()
		{
			editorPane.ConfirmClosing(false);
			editorPane.Dispose();
		}

		private void OnGUI()
		{
			if (editorPane != null) //OnGUI gets called before Awake??
				editorPane.DoGUILayout();
		}
	}
}