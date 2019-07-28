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
		#region Directions3D lookup tables

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

		#endregion

		#region Transform2D lookup tables

		private static Transform2D[] AllTransforms2D = Enum.GetValues(typeof(Transform2D))
														   .Cast<Transform2D>()
														   .Where(t => t != Transform2D.Count)
														   .ToArray();

		private static Dictionary<Transform2D, string> AllTransform2DNames =
			new Dictionary<Transform2D, string>() {
				{ Transform2D.None, "None" },
				{ Transform2D.Rotate90CW, "Rot 90" },
				{ Transform2D.Rotate180, "Rot 180" },
				{ Transform2D.Rotate270CW, "Rot 270" },
				{ Transform2D.FlipX, "Flip X" },
				{ Transform2D.FlipY, "Flip Y" },
				{ Transform2D.FlipDiag1, "Flip Diag1" },
				{ Transform2D.FlipDiag2, "Flip Diag2" }
			};
		private static Dictionary<string, Transform2D> AllTransform2DsByName =
			AllTransform2DNames.ToDictionary(kvp => kvp.Value, kvp => kvp.Key);

		#endregion


		public override string Description => "Faces";

		public Tileset3D_TileEditor ParentEditor;


		//The selected face to edit, on the tile.
		private int currentDirI = 0;
		private Directions3D currentDir
		{
			get { return AllDirs3D[currentDirI]; }
			set { currentDirI = AllDirs3D.IndexOf(value); }
		}

		//The "blueprint" used by the currently-selected tile face.
		private int currentFaceI = -1;
		private Tileset3D.FaceRef currentFaceRef
		{
			get { return ParentEditor.TileCopy.Faces[currentFaceI]; }
		}
		private Tileset3D.Face currentFace
		{
			get { return currentFaceRef.GetBlueprint(ParentEditor.Tileset); }
		}


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

			using (GUIBlock.Layout_HorizontalPad(0, -1))
				GUILayout.Label("Preview Transform", TilesetGUI.Style_Label_SmallTitle);

			//Define some helper macros.
			Action<Transform2D> drawLabel = t2D =>
			{
				var content = new GUIContent(AllTransform2DNames[t2D],
											 previewAnimTooltipPrefix + AllTransform2DNames[t2D]);
				GUILayout.Label(content, TilesetGUI.Style_Button_Green);
			};

			//Show the mouse-over items.
			using (GUIBlock.Layout_HorizontalPad())
				drawLabel(Transform2D.None);
			using (GUIBlock.Layout_HorizontalPad())
			{
				drawLabel(Transform2D.Rotate90CW);
				GUILayout.FlexibleSpace();
				drawLabel(Transform2D.Rotate180);
				GUILayout.FlexibleSpace();
				drawLabel(Transform2D.Rotate270CW);
			}
			using (GUIBlock.Layout_HorizontalPad())
			{
				drawLabel(Transform2D.FlipX);
				GUILayout.FlexibleSpace();
				drawLabel(Transform2D.FlipY);
				GUILayout.FlexibleSpace();
				drawLabel(Transform2D.FlipDiag1);
				GUILayout.FlexibleSpace();
				drawLabel(Transform2D.FlipDiag2);
			}

			//Check the tooltip to see if one is moused-over.
			string tooltip = GUI.tooltip;
			if (tooltip == null)
				tooltip = "";
			if (tooltip.StartsWith(previewAnimTooltipPrefix))
			{
				tooltip = tooltip.Substring(previewAnimTooltipPrefix.Length);
				Transform2D t2D;
				if (AllTransform2DsByName.TryGetValue(tooltip, out t2D))
					SetPreviewTransform(t2D);
			}
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

		private void SetPreviewTransform(Transform2D t2D)
		{
			//Find the 3D transformation that achieves the desired purpose.
			switch (t2D)
			{
				//TODO: Finish. Move into WFC_Helper.
				default: throw new NotImplementedException(t2D.ToString());
			}

			//Get the rotation and scale for that transformation.
			WFC_Helper.GetTransform(previewTargetTransform,
									out previewTargetRot, out previewTargetScale);
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
