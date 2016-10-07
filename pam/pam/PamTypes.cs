using System;
using System.Runtime.InteropServices;
//pam_appl.h postpones what type pam_handle_t is, presumably in order to leave that up to the particular version/implementation of Pam on the system? So just a blank definition.
	public class PamTypes{
	[StructLayout(LayoutKind.Sequential), Serializable]
	struct pam_handle_t{

	   }
	[StructLayout(LayoutKind.Sequential), Serializable]
	unsafe struct pam_message {
		int msg_style;
		char* *msg; 
	}
}

