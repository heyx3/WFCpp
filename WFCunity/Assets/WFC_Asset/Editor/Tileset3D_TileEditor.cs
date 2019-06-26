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
	public class Tileset3D_TileEditor : Tileset3DEditorPiece
	{
		public Tileset3D Tileset { get; private set; }
		public int TileIndex { get; private set; }

		public Tileset3D.Tile TileCopy { get; private set; }

		private EditorWindowScene tileScene = new EditorWindowScene();


		public Tileset3D_TileEditor() { }
		public Tileset3D_TileEditor(Tileset3D tileset, int tileIndex = 0)
		{
			Reset(tileset, tileIndex);
		}

		public void Reset(Tileset3D newTileset, int newTileIndex = 0)
		{
			Tileset = newTileset;
			TileIndex = newTileIndex;
			TileCopy = new Tileset3D.Tile(Tileset.Tiles[TileIndex]);
		}
		public override void Dispose()
		{
			base.Dispose();
			tileScene.Dispose();
		}

		protected override string Description =>
			"tile " + ((TileCopy == null || TileCopy.Prefab == null) ? "" : TileCopy.Prefab.name);

		public override void SaveChanges()
		{
			Tileset.Tiles[TileIndex] = TileCopy;

			base.SaveChanges();
		}
		public override void RevertChanges()
		{
			base.RevertChanges();

			TileCopy = new Tileset3D.Tile(Tileset.Tiles[TileIndex]);
		}

		public override void DoGUILayout()
		{
			

			base.DoGUILayout();
		}
	}
}