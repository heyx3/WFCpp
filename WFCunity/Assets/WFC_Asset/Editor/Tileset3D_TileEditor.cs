using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;

namespace WFC_CS.Editor
{
	/// <summary>
	/// The data structure that Tileset3D_TileEditor is editing.
	/// </summary>
	public class TileEditor_Data
	{
		public Tileset3D Tileset { get; private set; }
		public int TileIndex { get; private set; }

		public Tileset3D.Tile TileCopy { get; private set; }

		public TileEditor_Data()
		{
			Tileset = null;
			TileIndex = -1;
			TileCopy = new Tileset3D.Tile();
		}
		public TileEditor_Data(Tileset3D tileset, int tileIndex = 0)
		{
			Tileset = tileset;
			TileIndex = tileIndex;
			TileCopy = new Tileset3D.Tile(Tileset.Tiles[TileIndex]);
		}
	}


	/// <summary>
	/// A piece of the tileset editor window that edits a specific tile.
	/// </summary>
	public class Tileset3D_TileEditor : Tileset3DEditorPiece
	{
		public event Action OnDeleteButton;

		public TileEditor_Data Data { get; private set; } //TODO: Remove data structure; use fields individually.

		protected override string Description =>
			"tile " + ((Data.TileCopy == null || Data.TileCopy.Prefab == null) ? "" : Data.TileCopy.Prefab.name);


		public Tileset3D_TileEditor()
		{
			Data = new TileEditor_Data();
		}
		public Tileset3D_TileEditor(Tileset3D tileset, int tileIndex = 0)
		{
			Data = new TileEditor_Data(tileset, tileIndex);
		}


		public override void SaveChanges()
		{
			Data.Tileset.Tiles[Data.TileIndex] = Data.TileCopy;

			base.SaveChanges();
		}
		public override void RevertChanges()
		{
			base.RevertChanges();

			Data = new TileEditor_Data(Data.Tileset, Data.TileIndex);
		}

		public void DoGUILayout()
		{
			//TODO: Implement.
		}
	}
}