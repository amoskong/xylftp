//////////////////////////////////////////////////////////////////////////
// Copyright (C) Zhou Xiao-wei, Apr. 2007.
//	Implemented XylFTPMain.main().
// Hacked by Wang Cong, May. 2007.
// See AUTHORS and CREDITS to learn more.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////
import java.net.*;
import java.io.*;


/**
 *The main class.
 *@author ZHOU Xiao-Wei
 *@author WANG Cong
 *@version 1.11
 *@see XylFTPException
 */
public final class XylFTPMain{
	
	/**
	 *Builds an object.
	 */
	private static XylFTPCLI MyFTP;
	
	/**
	 *Enable verbose output.
	 */
	private static boolean EnableVerbose = false;
	
	/**
	 *Enable debugging output.
	 */
	private static boolean EnableDebug = false;

	/**
	 *Tests whether it is enable debug.
	*/
	public static boolean GetEnableDebug(){
		return EnableDebug;
	}

	/**
	 *Tests whether it is enable verbose.
	*/
	public static boolean GetEnableVerbose(){
		return EnableVerbose;
	}

	/**
	 *Sets the value of Enabledebug.
	 *@param Value the new value of Enabledebug.
	*/
	public static void SetEnableDebug(boolean Value){
		EnableDebug = Value;
	}

	/**
	 *Sets the value of Enableverbose.
	 *@param Value the new value of Enableverbose.
	*/
	public static void SetEnableVerbose(boolean Value){
		EnableVerbose = Value;
	}

	/**
	 *Logo used by this program.
	 */
	private final static String LOGO = "xylftp>";

	/**
	  *Parse the port number from echo.
	  *@param From the echo to be parsed
	  *@exception XylFTPException on parsing port error
	  *@see XylFTPException
	  *@return the number of the port
	  */
	private static int ParsePort(String From) throws XylFTPException{
		int i, j;
		int port1, port2;
		String t;
		String [] nums;
		i = From.indexOf("(");
		if (i==-1)
			throw new XylFTPException("", 1, "Invalid echo!");
		j = From.indexOf(")");
		if (j==-1)
			throw new XylFTPException("", 1, "Invalid echo!");
		t = From.substring(i+1, j);
		nums = t.split(",");
		port1 = Integer.parseInt(nums[4]);
		port2 = Integer.parseInt(nums[5]);
		return port1*256 + port2;
	}
	
	/**
	 *Parse the IP address from the echo.
	 *@param From the echo to be parsed
	 *@exception XylFTPException on parsing IP error
	 *@see XylFTPException
	 *@return all the numbers of the IP as a string
	 */
	private static String ParseIP(String From) throws XylFTPException{
		int i, j;
		String t;
		String [] nums;
		i = From.indexOf("(");
		if (i==-1)
			throw new XylFTPException("PASV", 1, "Invalid echo!");
		j = From.indexOf(")");
		if (j==-1)
			throw new XylFTPException("PASV", 1, "Invalid echo!");
		t = From.substring(i+1, j);
		nums = t.split(",");
		if (nums.length < 4)
			throw new XylFTPException("PASV", 1, "Invalid echo!");
		return (nums[0]+"."+nums[1]+"."+nums[2]+"."+nums[3]);

	}
	
	/**
	 *The main function.
	 */
	public static void main(String args[]) {
		int flag = 0;
		// 0 for standard read
		// 1 for read username 
		// 2 for read password
		// 3 for read help parameter
		char ch;
		boolean toConnectAtStart = false;
		MyFTP = new XylFTPCLI();

		for (int i = 0; i < args.length; i++) {
			if (flag == 0) { // standard read
				if (args[i].charAt(0) == '-' && args[i].charAt(1) == '-') { // long argument
					if (args[i].equals("--user")) {
						if (i < args.length - 1) {
							flag = 1; // Read username next time
						} else { // Nothing after --user indicates an error
							MyFTP.ShowUsage();
							System.exit(1);
						}
					} else if (args[i].equals("--help")) {
						if (i < args.length - 1 
							&& args[i + 1].charAt(0) != '-') { // Need to read help parameter
							flag = 3;
						} else {
							MyFTP.ShowUsage();
							System.exit(0);
						}
					} else if (args[i].equals("--password")) {
						if (i < args.length - 1) {
							flag = 2; // Read password next time
						} else { // Nothing after --password indicates an error
							MyFTP.ShowUsage();
							System.exit(1);
						}
					} else if (args[i].equals("--version")) {
						MyFTP.ShowVersion();
						System.exit(0);
					} else if (args[i].equals("--verbose")) {
						EnableVerbose = true;
					} else if (args[i].equals("--debug")) {
						EnableDebug = true;
					} else { // illegal long argument
						MyFTP.ShowUsage();
						System.exit(1);
					}

				} else if (args[i].charAt(0) == '-') { // short argument
					if (args[i].length() > 2 || args[i].length() <= 1) { // Short argument too long or too short
						MyFTP.ShowUsage();
						System.exit(1);
					} else {
						ch = args[i].charAt(1); // argument char
						switch (ch) {
						case 'u':
							if (i < args.length - 1) {
								flag = 1; // Read username next time
							} else { // Nothing after -u indicates an error
								MyFTP.ShowUsage();
								System.exit(1);
							}
							break;
						case 'h':
							if (i < args.length - 1
								&& args[i + 1].charAt(0) != '-') { // Need to read help parameter
								flag = 3;
							} else {
								MyFTP.ShowUsage();
								System.exit(0);
							}
							break;
						case 'p':
							if (i < args.length - 1) {
								flag = 2; // Read password next time
							} else { // Nothing after -p indicates an error
								MyFTP.ShowUsage();
								System.exit(1);
							}
							break;
						case 'V':
							MyFTP.ShowVersion();
							System.exit(0);
							break;
						case 'v':
							EnableVerbose = true;
							break;
						case 'd':
							EnableDebug = true;
							break;
						default: // illegal short argument
							MyFTP.ShowUsage();
							System.exit(1);
						}
					}
				} else {
					if (toConnectAtStart) { // illegal argument
						MyFTP.ShowUsage();
						System.exit(1);
					} else { // Command line contains host name
						toConnectAtStart = true;
						MyFTP.SetHost(args[i]);
					}
				}
			} else if (flag == 1) { // Read Username
				MyFTP.SetUserName(args[i]);
				flag = 0;
			} else if (flag == 2) { // Read password
				MyFTP.SetPassWord(args[i]);
				flag = 0;
			} else if (flag == 3) { // Read help parameter
				MyFTP.ShowHelp(args[i]);
				System.exit(0);
			}
		}

		String echo;
		String cmds[];
		int echoMeaning;
		int HostPort = 0;
		String HostIP = null;
		while (true) {
			try {
				if (toConnectAtStart) {
					try {
						MyFTP.OpenConnection();
						String s = MyFTP.GetEcho();
						if (s==null)
							throw new XylFTPException("xylftp", 0,
									"Cann't get an echo.");
						else
							System.out.println(s);
						while (s.charAt(3) == '-'){
							s = MyFTP.GetEcho();
							if (s==null)
								throw new XylFTPException("xylftp",
									0, "Can't get an echo.");
							if (EnableDebug)
								System.out.println("<---"+s);
							System.out.println(s);
						}
						MyFTP.SetStatus(1);
						cmds = MyFTP.MakeCommands("USER "
								+MyFTP.GetUserName()+"\r\n"
								+"PASS "+MyFTP.GetPassWord()
								+"\r\n");
					} finally {
						toConnectAtStart = false;
					}
				} else {
					System.out.print(LOGO);
					cmds = MyFTP.GetCommands();
				}
				if (cmds == null)
					continue;
FOR:
				for (int j = 0; j < cmds.length; j++) {
					if (EnableDebug)
						if (cmds[j].startsWith("PASS"))
							System.out.println("--->PASS ******");
						else
							System.out.println("--->"+cmds[j]);
					int restore = MyFTP.GetStatus();
					try{
						MyFTP.SendCommand(cmds[j]);
					} catch (IOException e) {
						throw new XylFTPException("xylftp", restore,
								e.getMessage());
					}
					if (cmds[j].startsWith("RETR")
							|| cmds[j].startsWith("LIST")
							|| cmds[j].startsWith("NLST")){
						MyFTP.SetStatus(3);
						try {
							if (MyFTP.IsPassive())
								MyFTP.OpenDataConnection(HostIP,
									HostPort);
							else
								MyFTP.OpenDataConnection();
						} catch (Exception e) {
							if (MyFTP.HasEcho()){
								String ec = MyFTP.GetEcho();
								if (EnableDebug)
									System.out.println("<---"+ec);
								MyFTP.ProcessEcho(ec);
							}
							throw new XylFTPException("xylftp", 2,
									e.getMessage());
						}
					}
					if (cmds[j].startsWith("STOR")){
						MyFTP.SetStatus(4);
						try {
							if (MyFTP.IsPassive())
								MyFTP.OpenDataConnection(HostIP, HostPort);
							else
								MyFTP.OpenDataConnection();
						} catch (Exception e) {
							throw new XylFTPException("", 2, e.getMessage());
						}
					}
					if (cmds[j].startsWith("PORT")){
						MyFTP.ReadyForActive();
					}

					boolean toGetEchoAgain;
					do {
						toGetEchoAgain = false;
						try {
							echo = MyFTP.GetEcho();
							if (echo==null)
								throw new XylFTPException("xylftp",
									0, "Can't get an echo.");
						} catch (IOException e) {
							throw new XylFTPException("xylftp", 0,
									e.getMessage());
						}
						if (!MyFTP.IsValidEcho(echo))
							throw new XylFTPException("xylftp", 0,
									"Invaild echo.");
						if (EnableDebug)
							System.out.println("<---"+echo);
						echoMeaning = MyFTP.ProcessEcho(echo);
						while (echoMeaning == 6) {
							echo = MyFTP.GetEcho();
							if (echo==null)
								throw new XylFTPException("xylftp",
									0, "Can't get an echo.");
							if (EnableDebug)
								System.out.println("<---"+echo);
							echoMeaning = MyFTP.ProcessEcho(echo);
						}
						switch (echoMeaning) {
						case -1:
							throw new XylFTPException("panic", 0,
									"Unknown mistake!");
						case 0:
							throw new XylFTPException("xylftp", 0,
									"Invalid format!");
						case 1: // Need data connection
							if (cmds[j].startsWith("RETR")
								|| cmds[j].startsWith("LIST")
								|| cmds[j].startsWith("NLST")){
								try {
									if (MyFTP.IsPassive())
										MyFTP.GetFilePassive();
									else
										MyFTP.GetFileActive();
									MyFTP.CloseDataConnection();
								} catch (Exception e) {
									throw new XylFTPException("", e.getMessage());
								} finally {
									MyFTP.SetStatus(2);
								}
								toGetEchoAgain = true;
							}
							if (cmds[j].startsWith("STOR")){
								try {
									if (MyFTP.IsPassive())
										MyFTP.SendFilePassive();
									else
										MyFTP.SendFileActive();
									MyFTP.CloseDataConnection();
								} finally {
									MyFTP.SetStatus(2);
								}
								toGetEchoAgain = true;
							}
							break;
						case 2:
							if (cmds[j].startsWith("PASS")){
								MyFTP.SetStatus(2);
								if (EnableDebug)
									System.out.println(
										"--->TYPE I");
								MyFTP.SendCommand("TYPE I");
								String s = MyFTP.GetEcho();
								if (s!=null &&
									MyFTP.IsValidEcho(s) &&
									s.substring(0,3).equals("200")){
									if (EnableDebug)
										System.out.println(
										"<---"+s);
									MyFTP.ProcessEcho(s);
									MyFTP.SetTransferMode(0);
								}
							}
							if (cmds[j].startsWith("QUIT")){
								MyFTP.SetStatus(0);
								MyFTP.CloseConnection();
								if (j+1 < cmds.length &&
									cmds[j+1].startsWith("QUIT"))
									System.exit(0);
							}
							if (cmds[j].startsWith("NLST")	//For 450
								|| cmds[j].startsWith("LIST")){
								MyFTP.SetStatus(2);
							}
							break FOR;
						case 3:
							if (cmds[j].startsWith("TYPE")){
								String []cmd = cmds[j].split("[\t ]+");
								if(cmd[1].equals("A"))
									MyFTP.SetTransferMode(1);
								if(cmd[1].equals("I"))
									MyFTP.SetTransferMode(0);
								break FOR;
							}
							if (cmds[j].startsWith("PASV")){
								MyFTP.SetPassive();
								HostIP = ParseIP(echo);
								HostPort = ParsePort(echo);
								if (EnableVerbose)
									System.out.println("Connect to "+HostIP+": "+HostPort);
							}
							if (cmds[j].startsWith("PORT")){
								MyFTP.SetActive();
							}
							break;
						case 4:
							j=0;
							break;
						case 5:
							if (cmds[j].startsWith("RETR")
								|| cmds[j].startsWith("STOR")) {
								MyFTP.SetStatus(2);
							}
							break FOR;
						} // switch
					} while (toGetEchoAgain); // do
				} // for
			} catch (XylFTPException e) {
				System.out.println(e.GetMessage());
				if (e.GetStatus()!=-1) {
					if (MyFTP.GetStatus()!=0 && e.GetStatus()==0)
						try {
							System.out.println("Connection closed!");
							MyFTP.CloseConnection();
						} catch (Exception ee) {
							System.out.println(ee.getMessage());
						}
					MyFTP.SetStatus(e.GetStatus());
				}
			} catch (Exception e) {
				System.out.println(e.getMessage());
			}

		} // while(true)
	}

}
