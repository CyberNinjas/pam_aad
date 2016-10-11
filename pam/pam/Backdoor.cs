using System;
using System.Runtime.InteropServices;
using static PamTypes;
public class Tester
{

	[DllImport("/home/cyberninjas/Documents/mono-pam/pam/pam/mypam.so", EntryPoint="pam_sm_authenticate")]
	static extern void pam_sm_authenticate(ref pam_handle_t pamh, int flags, int argc, string[] argv);
//See the following link for details regarding pam_get_user implementation pubs.opengroup.org/onlinepubs/8329799/pam_get_user.html
	[DllImport("/home/cyberninjas/Documents/mono-pam/pam/pam/mypam.so", EntryPoint="pam_get_user")]
	static extern void pam_get_user (ref pam_handle_t pamh, [MarshalAs (UnmanagedType.LPStr)] string user, [MarshalAsAttribute (UnmanagedType.LPStr)] string prompt);

	[DllImport("/home/cyberninjas/Documents/mono-pam/pam/pam/mypam.so", EntryPoint = "username_and_password")]
	static extern int username_and_password (ref pam_handle_t pamh, [MarshalAs (UnmanagedType.LPStr)] string username, [MarshalAsAttribute (UnmanagedType.LPStr)] string password);
	public static void Main(string[] args)
	{
		pam_handle_t t = new pam_handle_t ();
		Console.WriteLine ("Please provide your username: ");
		String username = Console.ReadLine();
		Console.WriteLine ("Please provide your password: ");
		String password = Console.ReadLine();
	    username_and_password (ref t, username, password);
	}
}