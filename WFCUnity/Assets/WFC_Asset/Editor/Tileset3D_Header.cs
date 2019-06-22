using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;


namespace WFC_CS.Editor
{
	public class Tileset3D_Header : Tileset3DEditorPiece<Tileset3D>
	{
		/// <summary>
		/// Raised when the tile with the given index was selected to be edited.
		/// </summary>
		public event Action<int> OnEditTile;
		/// <summary>
		/// Raised when the tile with the given index was selected to be edited.
		/// </summary>
		public event Action OnNewTile;


		private Tileset3D originalTileset;


		public Tileset3D_Header() { }
		public Tileset3D_Header(Tileset3D original)
		{
			originalTileset = original;
		}


		public override void SaveChanges()
		{
			EditorUtility.CopySerialized(Data, originalTileset);

			base.SaveChanges();
		}
		public override void RevertChanges()
		{
			base.RevertChanges();

			Data = UnityEngine.Object.Instantiate<Tileset3D>(originalTileset);
		}

		public override void DoGUILayout()
		{
			using (GUIBlock.Layout_Horizontal())
			{
				//TODO: Implement.
			}
		}
	}
}
