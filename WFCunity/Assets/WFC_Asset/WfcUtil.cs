using System;
using System.Collections.Generic;
using System.Linq;
using System.IO;
using System.Text;
using UnityEngine;

#if UNITY_EDITOR
using UnityEditor;
#endif


namespace WFC_CS
{
	public static class WfcUtil
	{
		public static int IndexOf<T>(this IEnumerable<T> items, Predicate<T> test)
		{
			int i = 0;
			foreach (T t in items)
				if (test(t))
					return i;
				else
					i += 1;

			return -1;
		}
		public static int LastIndexOf<T>(this IList<T> list, Predicate<T> test)
		{
			int i = list.Count - 1;
			while (i >= 0 && !test(list[i]))
				i -= 1;
			return i;
		}

		public static string[] SplitDir(this string path)
		{
			return path.Split(new char[] { '\\', '/' },
							  StringSplitOptions.RemoveEmptyEntries);
		}
		public static bool IsPathInsideFolder(this string path, string folderName)
		{
			var pathElements = Path.GetFullPath(path).SplitDir();
			var targetElements = folderName.SplitDir();

			for (int i = 0; i <= pathElements.Length - targetElements.Length; ++i)
			{
				//See if the target folder path starts at this element.
				int j = 0;
				while (j < targetElements.Length && pathElements[i + j] == targetElements[j])
					j += 1;

				if (j == targetElements.Length)
					return true;
			}

			return false;
		}
		public static string MakeRelativePath(this string path, string newStartFolder)
		{
			string[] paths = path.SplitDir();
			int i = paths.LastIndexOf(s => s == newStartFolder);

			string newPath = paths[i];
			for (i += 1; i < paths.Length; ++i)
				newPath += Path.DirectorySeparatorChar + paths[i];
			return newPath;
		}

		public static string TrimEnd(this string str, string toRemove)
		{
			if (str.Length >= toRemove.Length &&
				str.Substring(str.Length - toRemove.Length) == toRemove)
			{
				return str.Substring(0, str.Length - toRemove.Length);
			}

			return str;
		}

		public static float GetBoundingRadius(this Bounds b)
		{
			return b.extents.magnitude;
		}

	#if UNITY_EDITOR

	#endif
	}
}
