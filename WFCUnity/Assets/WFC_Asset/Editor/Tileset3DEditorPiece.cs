using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WFC_CS.Editor
{
	/// <summary>
	/// An editor for a specific part of the tileset data structure.
	/// Provides functionality for applying/reverting changes.
	/// </summary>
	public abstract class Tileset3DEditorPiece<T>
	{
		/// <summary>
		/// The item being edited.
		/// Should be a copy of the item from the original tileset instance.
		/// </summary>
		public T Data { get; protected set; }

		public bool HasUnsavedChanges { get; protected set; } = false;


		/// <summary>
		/// Saves this editor's changes to the edited item.
		/// Default behavior: calls "RevertChanges()",
		///     because a new copy of the data may have to be made
		///     after saving the current one.
		/// </summary>
		public virtual void SaveChanges()
		{
			RevertChanges();
		}
		/// <summary>
		/// Removes this editor's changes to the edited item since it was last saved.
		/// Default behavior: sets "HasUnsavedChanges" to false.
		/// </summary>
		public virtual void RevertChanges()
		{
			HasUnsavedChanges = false;
		}
		
		/// <summary>
		/// If there are any unsaved changes, asks the user and then potentially saves them.
		/// </summary>
		public ConfirmClosingDialog.Results TryToClose(bool canCancel)
		{
			if (!HasUnsavedChanges)
				return ConfirmClosingDialog.Results.DontSave;

			var result = ConfirmClosingDialog.Show(canCancel);
			switch (result)
			{
				case ConfirmClosingDialog.Results.Cancel: break;

				case ConfirmClosingDialog.Results.Save:
					SaveChanges();
					break;

				case ConfirmClosingDialog.Results.DontSave:
					RevertChanges();
					break;

				default: throw new NotImplementedException(result.ToString());
			}
			return result;
		}
		
		public abstract void DoGUILayout();
	}
}
