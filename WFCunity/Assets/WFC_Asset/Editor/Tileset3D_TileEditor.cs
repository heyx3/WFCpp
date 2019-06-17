using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;

namespace WFC_CS.Editor
{
	/// <summary>
	/// A piece of the tileset editor window that edits a specific tile.
	/// </summary>
	public class Tileset3D_TileEditor
	{
		public bool HasUnsavedChanges = false;

		public Tileset3D.Tile TileCopy { get; private set; }

		public Tileset3D Tileset { get; private set; }
		public int SelectedIndex { get; private set; }

		public event Action OnDeleteButton;


		public void SaveChanges()
		{
			Tileset.Tiles[SelectedIndex] = TileCopy;
			TileCopy = new Tileset3D.Tile(TileCopy);

			HasUnsavedChanges = false;
		}
		public void RevertChanges()
		{
			TileCopy = new Tileset3D.Tile(Tileset.Tiles[SelectedIndex]);

			HasUnsavedChanges = false;
		}

		/// <summary>
		/// If there are unsaved changes, asks the user to save them.
		/// </summary>
		/// <param name="canCancel">
		/// If true, the user can choose to cancel the action that caused this dialog.
		/// </param>
		public ConfirmClosingDialog.Results ConfirmClosing(bool canCancel)
		{
			if (HasUnsavedChanges)
				return ConfirmClosingDialog.Show(canCancel);
			else
				return ConfirmClosingDialog.Results.DontSave;
		}

		public void Reset(Tileset3D tileset = null, int tileIndex = -1)
		{
			HasUnsavedChanges = false;

			//If a tile index wasn't given, try to select the first element from the tileset.
			if (tileset != null && tileIndex < 0)
				tileIndex = tileset.Tiles.Count - 1;

			Tileset = tileset;
			SelectedIndex = tileIndex;

			bool clearScreen = (tileset == null || tileIndex < 0);
			if (clearScreen)
				TileCopy = null;
			else
				TileCopy = new Tileset3D.Tile(tileset.Tiles[tileIndex]);
		}


		public void DoGUILayout()
		{
			//TODO: Implement.
		}
	}
}