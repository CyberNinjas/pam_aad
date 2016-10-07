using System;
using System.Runtime.InteropServices;
using static PamTypes;
public class Tester
{

	[DllImport("/home/cyberninjas/Documents/mono-pam/pam/pam/mypam.so", EntryPoint="pam_sm_authenticate")]
	static extern void pam_sm_authenticate(ref pam_handle_t pamh, int flags, int argc, string[] argv);
	[DllImport("/home/cyberninjas/Documents/mono-pam/pam/pam/mypam.so")]
	static extern void pam_get_user (ref pam_handle_t pamh, string user, string prompt);

	public static void Main(string[] args)
	{
		pam_handle_t t = new pam_handle_t ();
		int placeholder = 1;
		pam_sm_authenticate(ref t, placeholder, args.Length, args);
	}
}