using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

using Transforms2D = WFC_CS.LowLevel.Transformations;

namespace WFC_CS
{
	public class Tileset3D : ScriptableObject
	{
		public Bounds TileBounds = new Bounds(Vector3.one * 0.5f, Vector3.one);

		#region Face data

		/// <summary>
		/// The "blueprint" for a tile face.
		/// Tiles can line up only if their touching faces come from the same blueprint.
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

		/// <summary>
		/// A specific face on a tile, using one of the Face "blueprints".
		/// </summary>
		[Serializable]
		public class FaceRef
		{
			/// <summary>
			/// The index of the Face data structure this instance is based on.
			/// </summary>
			public int BlueprintI;
			/// <summary>
			/// Applied to the four point ID's in the blueprint.
			/// </summary>
			public Transforms2D Transform = Transforms2D.None;

			public Face GetBlueprint(Tileset3D tileset) { return tileset.Faces[BlueprintI]; }
		}

		#endregion

		#region Tile data

		[Serializable]
		public class Tile
		{
			public GameObject Prefab;

			public List<LowLevel.Rotations3D> NormalSymmetries = new List<LowLevel.Rotations3D>() { LowLevel.Rotations3D.None },
											  InvertedSymmetries = new List<LowLevel.Rotations3D>();

			/// <summary>
			/// The faces of this tile, indexed by the Directions3D enum.
			/// </summary>
			public FaceRef[] Faces = new FaceRef[6];


			public Tile() { }
			public Tile(Tile toCopy)
			{
				Prefab = toCopy.Prefab;
				NormalSymmetries = toCopy.NormalSymmetries.ToList();
				InvertedSymmetries = toCopy.InvertedSymmetries.ToList();
				Faces = toCopy.Faces.ToArray();
			}
		}

		#endregion

		
		public List<Tile> Tiles = new List<Tile>();
		public List<Face> Faces = new List<Face>();
	}
}