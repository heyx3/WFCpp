using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;

namespace WFC_CS
{
	public static class WFC_Helper
	{
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
					rot.x = 90;
					break;
				case LowLevel.Rotations3D.AxisX_180:
					rot.x = 180;
					break;
				case LowLevel.Rotations3D.AxisX_270:
					rot.x = 270;
					break;
					
				case LowLevel.Rotations3D.AxisY_90:
					rot.y = 90;
					break;
				case LowLevel.Rotations3D.AxisY_180:
					rot.y = 180;
					break;
				case LowLevel.Rotations3D.AxisY_270:
					rot.y = 270;
					break;
					
				case LowLevel.Rotations3D.AxisZ_90:
					rot.z = 90;
					break;
				case LowLevel.Rotations3D.AxisZ_180:
					rot.z = 180;
					break;
				case LowLevel.Rotations3D.AxisZ_270:
					rot.z = 270;
					break;

				case LowLevel.Rotations3D.EdgesXa:
					rot.x = -90;
					rot.y = 180;
					break;
				case LowLevel.Rotations3D.EdgesXb:
					rot.x = 90;
					rot.y = 180;
					break;
				case LowLevel.Rotations3D.EdgesYa:
					rot.y = 90;
					rot.z = 180;
					break;
				case LowLevel.Rotations3D.EdgesYb:
					rot.y = -90;
					rot.z = 180;
					break;
				case LowLevel.Rotations3D.EdgesZa:
					rot.y = 180;
					rot.z = 90;
					break;
				case LowLevel.Rotations3D.EdgesZb:
					rot.y = 180;
					rot.z = -90;
					break;

				case LowLevel.Rotations3D.CornerAAA_120:
					rot.x = -90;
					rot.z = -90;
					break;
				case LowLevel.Rotations3D.CornerAAA_240:
					rot.x = 180;
					rot.y = -90;
					rot.z = -90;
					break;

				case LowLevel.Rotations3D.CornerABA_120:
					rot.y = 90;
					rot.z = -90;
					break;
				case LowLevel.Rotations3D.CornerABA_240:
					rot.x = 90;
					rot.y = -90;
					break;
					
				case LowLevel.Rotations3D.CornerBAA_120:
					rot.y = -90;
					rot.z = -90;
					break;
				case LowLevel.Rotations3D.CornerBAA_240:
					rot.x = -90;
					rot.y = 180;
					rot.z = -90;
					break;

				case LowLevel.Rotations3D.CornerBBA_120:
					rot.x = 90;
					rot.y = 90;
					break;
				case LowLevel.Rotations3D.CornerBBA_240:
					rot.x = 180;
					rot.y = 90;
					rot.z = -90;
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
	}
}