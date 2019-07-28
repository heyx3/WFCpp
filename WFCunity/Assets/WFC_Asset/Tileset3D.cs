using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace WFC_CS
{
	public class Tileset3D : ScriptableObject
	{
		public Bounds TileBounds = new Bounds(Vector3.one * 0.5f, Vector3.one);

		#region Tile data

		/// <summary>
		/// A side of a tile.
		/// </summary>
		[Serializable]
		public class Face
		{
			public int StartI;

			/// <summary>
			/// The IDs at each corner, indexed by the FacePoints enum (i.e. in world space).
			/// Should be relative to the "StartI" value
			///     (so to get the actual ID passed to the DLL, add StartI to each of these).
			/// </summary>
			public int[] PointIDs = new int[4]; //Indexed by the FacePoints enum.

			/// <summary>
			/// An identifier to make the UI more readable.
			/// </summary>
			public string Name;


			public Face() { }
			public Face(Face toCopy, int newStartI)
			{
				StartI = newStartI;
				PointIDs = toCopy.PointIDs.ToArray();
				Name = toCopy.Name;
			}
		}

		[Serializable]
		public class Tile
		{
			public GameObject Prefab;

			public List<LowLevel.Rotations3D> NormalSymmetries = new List<LowLevel.Rotations3D>() { LowLevel.Rotations3D.None },
											  InvertedSymmetries = new List<LowLevel.Rotations3D>();

			public int[] FaceIndices = new int[6]; //Indexed by the Directions3D enum.

			public Tile() { }
			public Tile(Tile toCopy)
			{
				Prefab = toCopy.Prefab;
				NormalSymmetries = toCopy.NormalSymmetries.ToList();
				InvertedSymmetries = toCopy.InvertedSymmetries.ToList();
				FaceIndices = toCopy.FaceIndices.ToArray();
			}


			public Face GetFace(Tileset3D tileset, WFC_CS.LowLevel.Directions3D side)
			{
				return tileset.Faces[FaceIndices[(int)side]];
			}
		}

		public List<Tile> Tiles = new List<Tile>();
		public List<Face> Faces = new List<Face>();

		#endregion
	}
}