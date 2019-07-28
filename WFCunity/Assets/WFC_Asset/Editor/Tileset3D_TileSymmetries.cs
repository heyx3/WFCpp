using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;
using Rotations3D = WFC_CS.LowLevel.Rotations3D;

namespace WFC_CS.Editor
{
	public class Tileset3D_TileSymmetries : Tileset3DEditorPiece
	{
		private static Rotations3D[] AllRotations = Enum.GetValues(typeof(Rotations3D))
														.Cast<Rotations3D>()
														.Where(r => r != Rotations3D.Count)
														.ToArray();

		private static Dictionary<Rotations3D, string> AllRotationNames   = AllRotations.ToDictionary(r => r, r => r.ToString());
		private static Dictionary<string, Rotations3D> AllRotationsByName = AllRotationNames.ToDictionary(kvp => kvp.Value, kvp => kvp.Key);


		public override string Description => "Symmetries";

		public Tileset3D_TileEditor ParentEditor;

		private Vector2 scroll_normal = Vector2.zero,
						scroll_inverted = Vector2.zero;
		private const string invertedTooltipPrefix = "inv_";

		private Vector3 targetScale = Vector3.one;
		private Quaternion targetRot = Quaternion.identity;
		private WFC_CS.LowLevel.Transform3D targetTransform = new LowLevel.Transform3D();

		private const float animSpeed = 0.2f;
		private const float animResetDelay = 0.5f;

		private float timeTillAnimReset = 0.0f;


		public override void DoGUILayout()
		{
			using (GUIBlock.Layout_Horizontal())
			{
				//Show the normal and inverted symmetry lists.
				for (int i = 0; i < 2; ++i)
				{
					List<Rotations3D> symmetryList;
					string tooltipPrefix;
					GUIBlock scrollObj;
					string title;

					if (i == 0)
					{
						symmetryList = ParentEditor.TileCopy.NormalSymmetries;
						tooltipPrefix = "";
						title = "Normal";

						scrollObj = GUIBlock.Layout_Scroll(ref scroll_normal);
					}
					else
					{
						symmetryList = ParentEditor.TileCopy.InvertedSymmetries;
						tooltipPrefix = invertedTooltipPrefix;
						title = "Inverted";

						scrollObj = GUIBlock.Layout_Scroll(ref scroll_inverted);
					}

					GUILayout.Label(title, TilesetGUI.Style_Label_SmallTitle);
					using (scrollObj)
					{
						int rotI = -1;
						foreach (var rot in AllRotations)
						{
							rotI += 1;

							//Don't show the button for no-op symmetry.
							if (i == 0 && rot == Rotations3D.None)
								continue;

							//Insert spaces to separate different categories of rotations.
							if (rotI == 1 || rotI == 10 || rotI == 16)
								GUILayout.Space(15);

							string rotStr = AllRotationNames[rot];
							bool wasEnabled = symmetryList.Contains(rot);
							if (GUILayout.Button(new GUIContent(rotStr, tooltipPrefix + rotStr),
												 wasEnabled ?
													 TilesetGUI.Style_Button_Green :
													 TilesetGUI.Style_Button_Red))
							{
								if (wasEnabled)
									symmetryList.Remove(rot);
								else
									symmetryList.Add(rot);

								HasUnsavedChanges = true;
							}
						}
					}
				}
			}

			if (GUILayout.Button("Choose Inverted from Normal"))
			{
				HasUnsavedChanges = true;

				ParentEditor.TileCopy.InvertedSymmetries.Clear();
				ParentEditor.TileCopy.InvertedSymmetries.AddRange(ParentEditor.TileCopy.NormalSymmetries);
			}

			
			//Check the tooltip to see what is currently selected,
			//    then apply the selected transform.

			string tooltip = GUI.tooltip;
			if (tooltip == null)
				tooltip = "";

			//Check if the current transform is inverted.
			targetTransform.Invert = false;
			if (tooltip.StartsWith(invertedTooltipPrefix))
			{
				targetTransform.Invert = true;
				tooltip = tooltip.Substring(invertedTooltipPrefix.Length);
			}

			//Parse the rotation.
			Rotations3D selectedRot;
			if (AllRotationsByName.TryGetValue(tooltip, out selectedRot))
			{
				targetTransform.Rot = selectedRot;
				WFC_Helper.GetTransform(targetTransform, out targetRot, out targetScale);
				timeTillAnimReset = animResetDelay;
			}
			//If the tooltip doesn't represent a rotation, the tile should be un-transformed.
			else
			{
				if (timeTillAnimReset <= 0.0f)
				{
					targetScale = Vector3.one;
					targetRot = Quaternion.identity;
				}
			}

			base.DoGUILayout();
		}
		public override void Update()
		{
			//Animate the tile's transform.
			if (ParentEditor.PrefabPreviewObj != null)
			{
				var tr = ParentEditor.PrefabPreviewObj.transform;
				if (tr.rotation != targetRot || tr.localScale != targetScale)
				{
					tr.rotation = Quaternion.Slerp(tr.rotation, targetRot, animSpeed);
					tr.localScale = Vector3.Lerp(tr.localScale, targetScale, animSpeed);
					OwnerWindow.Repaint();
				}
			}
			
			//Update the counter to resetting the prefab's transform.
			timeTillAnimReset -= Time.deltaTime;

			base.Update();
		}

		protected override void OnDisabled()
		{
			base.OnDisabled();

			//Reset the tile's transform.
			if (ParentEditor.PrefabPreviewObj != null)
			{
				var tr = ParentEditor.PrefabPreviewObj.transform;
				tr.localScale = Vector3.one;
				tr.rotation = Quaternion.identity;
			}
		}
	}
}
