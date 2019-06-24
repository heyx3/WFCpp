﻿using System;
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


		private Tileset3D_Header editorPane;


		private void Awake()
		{
			//If a Tileset3D is already selected, load that one automatically.
			Debug.Log("Awake"); //Make sure this doesn't happen unexpectedly!
			var selectedTilesets = Selection.GetFiltered<Tileset3D>(SelectionMode.Assets);
			var tileset = (selectedTilesets.Length == 1) ? selectedTilesets[0] : null;

			//We're always working with copies of the original asset,
			//    so no harm in throwing exceptions.
			Assert.raiseExceptions = true;

			//Set up the UI panels.
			editorPane = new Tileset3D_Header(tileset);
		}
		private void OnDestroy()
		{
			editorPane.TryToClose(false);
		}

		private void OnGUI()
		{
			editorPane.DoGUILayout();
		}
	}
}