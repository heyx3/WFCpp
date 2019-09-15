using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace WFC_CS
{
	public static class WFC_Helper
	{
		public static void GetTransform(LowLevel.Directions3D face,
										LowLevel.Transformations faceTransform,
										LowLevel.Transform3D outTransform3D)
		{
			Dictionary<LowLevel.Transformations,
					   (bool, LowLevel.Rotations3D)> innerLookup;
			if (lookup_FaceTransformToCubeTransform.TryGetValue(face, out innerLookup))
			{
				(bool, LowLevel.Rotations3D) value;
				if (innerLookup.TryGetValue(faceTransform, out value))
				{
					outTransform3D.Invert = value.Item1;
					outTransform3D.Rot = value.Item2;
				}
				else
				{
					throw new NotImplementedException(faceTransform.ToString());
				}
			}
			else
			{
				throw new NotImplementedException(face.ToString());
			}
		}

		public static void GetTransform(LowLevel.Transform3D transform,
									    out Quaternion rot, out Vector3 scale)
		{
			//In both Unity and WFC, the inversion is applied before the rotation.
			scale = Vector3.one;
			if (transform.Invert)
				scale = -scale;

			Vector3 rotEuler = Vector3.zero;
			switch (transform.Rot)
			{
				case LowLevel.Rotations3D.None:
					break;

				case LowLevel.Rotations3D.AxisX_90:
					rotEuler.x = 90;
					break;
				case LowLevel.Rotations3D.AxisX_180:
					rotEuler.x = 180;
					break;
				case LowLevel.Rotations3D.AxisX_270:
					rotEuler.x = 270;
					break;
					
				case LowLevel.Rotations3D.AxisY_90:
					rotEuler.y = 90;
					break;
				case LowLevel.Rotations3D.AxisY_180:
					rotEuler.y = 180;
					break;
				case LowLevel.Rotations3D.AxisY_270:
					rotEuler.y = 270;
					break;
					
				case LowLevel.Rotations3D.AxisZ_90:
					rotEuler.z = 90;
					break;
				case LowLevel.Rotations3D.AxisZ_180:
					rotEuler.z = 180;
					break;
				case LowLevel.Rotations3D.AxisZ_270:
					rotEuler.z = 270;
					break;

				case LowLevel.Rotations3D.EdgesXa:
					rotEuler.x = -90;
					rotEuler.y = 180;
					break;
				case LowLevel.Rotations3D.EdgesXb:
					rotEuler.x = 90;
					rotEuler.y = 180;
					break;
				case LowLevel.Rotations3D.EdgesYa:
					rotEuler.y = 90;
					rotEuler.z = 180;
					break;
				case LowLevel.Rotations3D.EdgesYb:
					rotEuler.y = -90;
					rotEuler.z = 180;
					break;
				case LowLevel.Rotations3D.EdgesZa:
					rotEuler.y = 180;
					rotEuler.z = 90;
					break;
				case LowLevel.Rotations3D.EdgesZb:
					rotEuler.y = 180;
					rotEuler.z = -90;
					break;

				case LowLevel.Rotations3D.CornerAAA_120:
					rotEuler.x = -90;
					rotEuler.z = -90;
					break;
				case LowLevel.Rotations3D.CornerAAA_240:
					rotEuler.x = 180;
					rotEuler.y = -90;
					rotEuler.z = -90;
					break;

				case LowLevel.Rotations3D.CornerABA_120:
					rotEuler.y = 90;
					rotEuler.z = -90;
					break;
				case LowLevel.Rotations3D.CornerABA_240:
					rotEuler.x = 90;
					rotEuler.y = -90;
					break;
					
				case LowLevel.Rotations3D.CornerBAA_120:
					rotEuler.y = -90;
					rotEuler.z = -90;
					break;
				case LowLevel.Rotations3D.CornerBAA_240:
					rotEuler.x = -90;
					rotEuler.y = 180;
					rotEuler.z = -90;
					break;

				case LowLevel.Rotations3D.CornerBBA_120:
					rotEuler.x = 90;
					rotEuler.y = 90;
					break;
				case LowLevel.Rotations3D.CornerBBA_240:
					rotEuler.x = 180;
					rotEuler.y = 90;
					rotEuler.z = -90;
					break;

				default: throw new NotImplementedException(transform.Rot.ToString());
			}
			rot = Quaternion.Euler(rotEuler);
		}
		public static void ApplyTransform(LowLevel.Transform3D transform,
										  UnityEngine.Transform unityTransform)
		{
			Quaternion rot;
			Vector3 scale;
			GetTransform(transform, out rot, out scale);

			unityTransform.localRotation = rot;
			unityTransform.localScale = scale;
		}


		#region Lookup tables
		private static Dictionary<LowLevel.Directions3D,
								  Dictionary<LowLevel.Transformations,
								  (bool, LowLevel.Rotations3D)>>
			lookup_FaceTransformToCubeTransform =
			new Dictionary<LowLevel.Directions3D, Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>>()
			{
				//TODO: Implement.
				{ LowLevel.Directions3D.MinX,
					new Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>()
					{
						//{ LowLevel.Transformations.Rotate90CW,
						//	  (false, LowLevel.Rotations3D.AxisX_90) },
					}
				},
				{ LowLevel.Directions3D.MaxX,
					new Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>()
					{
						//{ LowLevel.Transformations.Rotate90CW,
						//	  (false, LowLevel.Rotations3D.AxisX_270) },
					}
				},
				{ LowLevel.Directions3D.MinY,
					new Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>()
					{
						//{ LowLevel.Transformations.Rotate90CW,
						//	  (false, LowLevel.Rotations3D.AxisX_270) },
					}
				},
				{ LowLevel.Directions3D.MaxY,
					new Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>()
					{
						//{ LowLevel.Transformations.Rotate90CW,
						//	  (false, LowLevel.Rotations3D.AxisX_270) },
					}
				},
				{ LowLevel.Directions3D.MinZ,
					new Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>()
					{
						//{ LowLevel.Transformations.Rotate90CW,
						//	  (false, LowLevel.Rotations3D.AxisX_270) },
					}
				},
				{ LowLevel.Directions3D.MaxZ,
					new Dictionary<LowLevel.Transformations, (bool, LowLevel.Rotations3D)>()
					{
						//{ LowLevel.Transformations.Rotate90CW,
						//	  (false, LowLevel.Rotations3D.AxisX_270) },
					}
				}
			};
		#endregion
	}
}