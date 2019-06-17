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


		private Tileset3D tilesetCopy = null;
		private Tileset3D tilesetOriginal = null;

		private Tileset3D_TileEditor pane_tileEditor = new Tileset3D_TileEditor();

		private bool hasUnsavedChanges = false;


		private void Awake()
		{
			//If a Tileset3D is already selected, load that one automatically.
			Debug.Log("Awake"); //Make sure this doesn't happen unexpectedly!
			var selectedTilesets = Selection.GetFiltered<Tileset3D>(SelectionMode.Assets);
			if (selectedTilesets.Length == 1)
				Reset(selectedTilesets[0]);

			//We're always working with copies of the original asset,
			//    so no harm in throwing exceptions.
			Assert.raiseExceptions = true;
		}
		private void OnDestroy()
		{
			//Handle unsaved changes.
			var whatToDo = pane_tileEditor.ConfirmClosing(false);
			switch (whatToDo)
			{
				case ConfirmClosingDialog.Results.Save:
					pane_tileEditor.SaveChanges();
					SaveChanges();
					break;

				case ConfirmClosingDialog.Results.DontSave:
					Reset();
					break;

				default: throw new NotImplementedException(whatToDo.ToString());
			}
		}

		private void OnGUI()
		{
			//TODO: Implement.
		}


		public void SaveChanges()
		{
			pane_tileEditor.SaveChanges();

			Assert.IsNotNull(tilesetCopy);
			EditorUtility.CopySerialized(tilesetCopy, tilesetOriginal);

			hasUnsavedChanges = false;
		}
		public void RevertChanges()
		{
			if (tilesetCopy != null)
				Destroy(tilesetCopy);
			tilesetCopy = Instantiate(tilesetOriginal);
			
			pane_tileEditor.Reset(tilesetCopy, pane_tileEditor.SelectedIndex);
			
			hasUnsavedChanges = false;
		}

		public ConfirmClosingDialog.Results ConfirmClosing(bool canCancel)
		{
			if (hasUnsavedChanges)
				return ConfirmClosingDialog.Show(canCancel);
			else
				return ConfirmClosingDialog.Results.DontSave;
		}

		public void Reset(Tileset3D asset = null)
		{
			tilesetOriginal = asset;

			//Set up the copy.

			if (tilesetCopy != null)
				Destroy(tilesetCopy);

			if (tilesetOriginal == null)
				tilesetCopy = null;
			else
				tilesetCopy = Instantiate(tilesetOriginal);
			
			pane_tileEditor.Reset(tilesetCopy);
		}
	}
}