using System;
using System.Collections.Generic;
using System.Linq;
using UnityEngine;
using UnityEditor;


namespace WFC_CS.Editor
{
	/// <summary>
	/// 
	/// </summary>
	public static class TilesetEditorGUI
	{
		public static GUIStyle Style_Label_Title
		{
			get
			{
				if (style_label_title == null)
				{
					style_label_title = new GUIStyle(GUI.skin.label);
					style_label_title.fontSize = 24;
				}
				return style_label_title;
			}
		}
		public static GUIStyle Style_Label_Button
		{
			get
			{
				if (style_label_button == null)
				{
					style_label_button = new GUIStyle(GUI.skin.button);
					style_label_button.fontSize = 12;
				}
				return style_label_button;
			}
		}
		public static GUIStyle Style_Button_Tab
		{
			get
			{
				if (style_button_tab == null)
				{
					style_button_tab = new GUIStyle(GUI.skin.toggle);
					style_button_tab.fontSize = 18;
				}
				return style_button_tab;
			}
		}
		public static GUIStyle Style_Button_Green
		{
			get
			{
				if (style_button_green == null)
				{
					tex_pixel_green = new Texture2D(1, 1);
					tex_pixel_green.SetPixel(0, 0, new Color(0.5f, 1, 0.5f));
					tex_pixel_green.Apply(false, false);

					tex_pixel_greenSelected = new Texture2D(1, 1);
					tex_pixel_greenSelected.SetPixel(0, 0, new Color(0.0f, 1, 0.0f));
					tex_pixel_greenSelected.Apply(false, false);

					tex_pixel_greenHover = new Texture2D(1, 1);
					tex_pixel_greenHover.SetPixel(0, 0, new Color(0.75f, 0.9f, 0.2f));
					tex_pixel_greenHover.Apply(false, false);

					style_button_green = new GUIStyle(GUI.skin.button);
					style_button_green.fontStyle = FontStyle.Bold;
					style_button_green.fontSize = 15;

					style_button_green.normal.background = tex_pixel_green;
					style_button_green.normal.textColor = Color.black;

					style_button_green.active.background = tex_pixel_greenSelected;
					style_button_green.active.textColor = Color.black;

					style_button_green.hover.background = tex_pixel_greenHover;
					style_button_green.hover.textColor = Color.black;

				}
				return style_button_green;
			}
		}
		public static GUIStyle Style_Button_Red
		{
			get
			{
				if (style_button_red == null)
				{
					//Copy the green style for the most part.
					var greenStyle = Style_Button_Green;

					tex_pixel_red = new Texture2D(1, 1);
					tex_pixel_red.SetPixel(0, 0, SwapRG(tex_pixel_green.GetPixel(0, 0)));
					tex_pixel_red.Apply(false, false);

					tex_pixel_redSelected = new Texture2D(1, 1);
					tex_pixel_redSelected.SetPixel(0, 0, SwapRG(tex_pixel_greenSelected.GetPixel(0, 0)));
					tex_pixel_redSelected.Apply(false, false);

					tex_pixel_redHover = new Texture2D(1, 1);
					tex_pixel_redHover.SetPixel(0, 0, SwapRG(tex_pixel_greenHover.GetPixel(0, 0)));
					tex_pixel_redHover.Apply(false, false);

					style_button_red = new GUIStyle(greenStyle);
					style_button_red.normal.background = tex_pixel_red;
					style_button_red.active.background = tex_pixel_redSelected;
					style_button_red.hover.background = tex_pixel_redHover;

				}
				return style_button_red;
			}
		}
		//TODO: Finish. Test params.

		private static GUIStyle style_label_title,
								style_label_button,
								style_button_tab,
								style_button_green,
							    style_button_red,
								style_button_bigText,
								style_textbox,
								style_checkbox;
		private static Texture2D tex_pixel_green, tex_pixel_greenSelected, tex_pixel_greenHover,
								 tex_pixel_red, tex_pixel_redSelected, tex_pixel_redHover;

		private static Color SwapRG(Color rgba) { return new Color(rgba.g, rgba.r, rgba.b, rgba.a); }
	}
}
