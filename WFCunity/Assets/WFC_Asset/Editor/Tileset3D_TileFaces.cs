using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;

using Transform2D = WFC_CS.LowLevel.Transformations;
using Rotations3D = WFC_CS.LowLevel.Rotations3D;
using Directions3D = WFC_CS.LowLevel.Directions3D;


namespace WFC_CS.Editor
{
	public class Tileset3D_TileFaces : Tileset3DEditorPiece
	{
		private static Directions3D[] AllDirs3D = Enum.GetValues(typeof(Directions3D))
													  .Cast<Directions3D>()
													  .ToArray();

		private static Dictionary<Directions3D, string> AllDirectionNames =
		    new Dictionary<Directions3D, string>() {
				{ Directions3D.MinX, "-X" },
				{ Directions3D.MaxX, "+X" },
				{ Directions3D.MinY, "-Y" },
				{ Directions3D.MaxY, "+Y" },
				{ Directions3D.MinZ, "-Z" },
				{ Directions3D.MaxZ, "+Z" }
			};
		private static Dictionary<string, Directions3D> AllDirectionsByName =
		    AllDirectionNames.ToDictionary(kvp => kvp.Value, kvp => kvp.Key);


		public override string Description => "Faces";

		public Tileset3D_TileEditor ParentEditor;


		private Tileset3D.Face currentFace;

		private int currentDirI = 0;
		private int currentFaceI = -1;

		#region Transform Preview

		private WFC_CS.LowLevel.Transform3D previewTargetTransform =
			new WFC_CS.LowLevel.Transform3D();
		private Vector3 previewTargetScale = Vector3.one;
		private Quaternion previewTargetRot = Quaternion.identity;
			
		private const float previewAnimSpeed = 0.2f;
		private const float previewAnimResetDelay = 0.5f;

		private float previewAnimResetTimer = 0.0f;

		private string previewAnimTooltipPrefix = "previewAnim_";


		private void DoGUILayout_PreviewTransforms()
		{
			//Show the GUI, and give each mouse-over item a tooltip.
			//TODO: Implement.

			//Check the tooltip to see if one is highlighted.
			//TODO: Implement.
		}
		private void Update_PreviewTransforms()
		{
			//Animate the tile's transform.
			if (ParentEditor.PrefabPreviewObj != null)
			{
				var tr = ParentEditor.PrefabPreviewObj.transform;
				if (tr.rotation != previewTargetRot || tr.localScale != previewTargetScale)
				{
					tr.rotation = Quaternion.Slerp(tr.rotation, previewTargetRot,
												   previewAnimSpeed);
					tr.localScale = Vector3.Lerp(tr.localScale, previewTargetScale,
												 previewAnimSpeed);
					OwnerWindow.Repaint();
				}
			}

			//Update the counter to resetting the prefab's transform.
			previewAnimResetTimer -= Time.deltaTime;
		}
		private void OnDisabled_PreviewTransforms()
		{
			//Reset the tile's transform.
			if (ParentEditor.PrefabPreviewObj != null)
			{
				var tr = ParentEditor.PrefabPreviewObj.transform;
				tr.localScale = Vector3.one;
				tr.rotation = Quaternion.identity;
			}
		}

		#endregion


		public override void DoGUILayout()
		{
			//TODO: Finish.

			DoGUILayout_PreviewTransforms();

			base.DoGUILayout();
		}
		public override void Update()
		{
			Update_PreviewTransforms();

			base.Update();
		}
		protected override void OnDisabled()
		{
			OnDisabled_PreviewTransforms();
			
			base.OnDisabled();
		}
	}
}
