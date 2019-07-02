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

		[Serializable]
		public class Tile
		{
			public GameObject Prefab;

			public List<LowLevel.Rotations3D> NormalSymmetries = new List<LowLevel.Rotations3D>(),
											  InvertedSymmetries = new List<LowLevel.Rotations3D>();

			public int[] FaceIDIndices = new int[6]; //Indexed by the Directions3D enum.

			public Tile() { }
			public Tile(Tile toCopy)
			{
				Prefab = toCopy.Prefab;
				NormalSymmetries = toCopy.NormalSymmetries.ToList();
				InvertedSymmetries = toCopy.InvertedSymmetries.ToList();
				FaceIDIndices = toCopy.FaceIDIndices.ToArray();
			}
		}

		public List<Tile> Tiles = new List<Tile>();

		#endregion

		#region FaceID data

		[Serializable]
		public class FaceIDs
		{
			public int[] IDs = new int[4]; //Indexed by the FacePoints enum.
		}
		public List<FaceIDs> FaceIDSets = new List<FaceIDs>();

		public FaceIDs GenerateNewFace()
		{
			int firstNewID = FaceIDSets.Max(f => f.IDs.Max()) + 1;
			return new FaceIDs()
			{
				IDs = new int[4] { firstNewID, firstNewID + 1, firstNewID + 2, firstNewID + 3 }
			};
		}

		#endregion
	}
}