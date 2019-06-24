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
	/// Also has child instances for editing sub-pieces of the data.
	/// </summary>
	public abstract class Tileset3DEditorPiece
	{
		public bool HasUnsavedChanges { get; protected set; } = false;

		public List<Tileset3DEditorPiece> Children { get; protected set; } = new List<Tileset3DEditorPiece>();


		/// <summary>
		/// Saves this editor's changes to the edited item.
		/// Default behavior: calls "SaveChanges()" on all children,
		///     then calls "RevertChanges()" because
		///     a new copy of the data may have to be made after saving the current one.
		/// </summary>
		public virtual void SaveChanges()
		{
			foreach (var child in Children)
				child.SaveChanges();

			RevertChanges();
		}
		/// <summary>
		/// Removes this editor's changes to the edited item since it was last saved.
		/// Default behavior: calls "RevertChanges()" on all children,
		///     then sets "HasUnsavedChanges" to false.
		/// </summary>
		public virtual void RevertChanges()
		{
			foreach (var child in Children)
				child.RevertChanges();

			HasUnsavedChanges = false;
		}
		
		/// <summary>
		/// If there are any unsaved changes, asks the user and then potentially saves them.
		/// </summary>
		public ConfirmClosingDialog.Results TryToClose(bool canCancel)
		{
			//First ask all children to close.
			//If any of them want to cancel, we will cancel.
			foreach (var child in Children)
			{
				var childResult = child.TryToClose(canCancel);
				if (childResult == ConfirmClosingDialog.Results.Cancel)
					return ConfirmClosingDialog.Results.Cancel;
			}


			if (!HasUnsavedChanges)
				return ConfirmClosingDialog.Results.DontSave;

			var msg = "Do you want to save your changes to " + Description + "?";
			var result = ConfirmClosingDialog.Show(canCancel,msg);
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


		/// <summary>
		/// Used in the TryToClose() method. For example, a value of "tile 'MyTile'"
		///     results in the message "Do you want to save your changes to tile 'MyTile'?"
		/// </summary>
		protected abstract string Description { get; }
	}
}
