//////////////////////////////////////////////////////////////////////////
// Copyright (C) Kong Jian-jun, Apr. 2007.
//	Implemented XylFTPCLI.ShowHelp(), XylFTPCLI.ProcessEcho(), etc.
// Copyright (C) Wang Cong, Apr. 2007.
//	Implemented XylFTPCLI.ParseInput().
// Hacked by Zhou Xiao-wei, May. 2007.
// See AUTHORS and CREDITS to learn more.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////

import java.net.*;
import java.io.*;

/**
 *This is class Command-line interface.
 *Extends XylFTPConnection.
 *Implements XylFTPInterface.
 *@see XylFTPConnection
 *@see XylFTPInterface
 *@author WANG Cong, KONG Jian-Jun
 *@version 1.14
 */
public class XylFTPCLI extends XylFTPConnection implements XylFTPInterface{
	/**
	 *The username.
	 */
	private String UserName;
	
	/**
	 *The password.
	 */
	private String PassWord;
	
	/**
	 *Presents the input of user.
	 */
	private String UserInput;

	/**
	 *The current working directory on local host.
	 */
	private File CurrentDir;

	/**
	 *Sets the default password.
	 */
	private final String DefaultPass = "xylftpuser@xylftp";

	/**
	 *Commands list.
	 */
	private String[] commands = {"help", "?", "ls", "lls",
	"dir", "get", "put", "cwd", "cd", "lcwd", "lcd", "pwd",
	"lpwd", "passive", "cdup", "lcdup", "quit", "bye",
	"open", "close", "user", "!!", "delete", "rmdir",
	"mkdir", "chmod", "size", "rename", "type", "status",
	"quote", "verbose", "debug"};

	/**
	 *Initializes some values.
	 */
	XylFTPCLI(){
		super();
		UserName = "anonymous";
		PassWord = DefaultPass;
		File f = new File(".");
		try {
			CurrentDir = new File(f.getCanonicalPath());
		} catch (Exception e) {
			System.out.println("Fatal: Can't get current directory!");
			System.exit(-1);
		}
	}
	
	/**
	 *Sets the username.
	 *@param NewName the new user name.
	 */
	public void SetUserName(String NewName){
		UserName = NewName;
	}
	
	/**
	 *Returns the username.
	 *@return username
	 */
	public String GetUserName(){
		return UserName;
	}
	
	/**
	 *Sets the password.
	 *@param NewPass the new password.
	 */
	public void SetPassWord(String NewPass){
		PassWord = NewPass;
	}
	
	/**
	 *Gets the password.
	 *@return password
	 */
	public String GetPassWord(){
		return PassWord;
	}
	
	/**
	 *Splits the command string into commands list.
	 *@param Command
	 *@return the string of command
	 */
	public String[] MakeCommands(String Command){
		if(Command == null)
			return null;
		String[] nstr;
		nstr = Command.split("\r\n");
		return nstr;
	}
	
	/**
	 *Counts the number of arguments.
	 *@param Str the input
	 *@return the number of arguments in Str
	 */
	private int CountArgs(String Str){
		String[] nstr;
		nstr = Str.split("[\t ]+");
		return nstr.length;
	}

	/**
	 *Look up the commands list, recognize which command matches.
	 *@return the index of the command, -1 on failure.
	 */
	private int LookupCommands(String input) throws Exception{
		int i = 0;
		for (; i< commands.length; i++) {
			if (StartsOnlyWith(input, commands[i]))
				return i;
		}
		//If not found, try again for incomplete input.
		boolean found_one = false;
		int tmp = -1;
		for (i = 0; i< commands.length; i++) {
			if (commands[i].startsWith(input.toLowerCase())) {
				if (found_one) {
					return -1;
				} else {
					found_one = true;
					tmp = i;
				}
			}
		}
		if (found_one)
			return tmp;
		else
			return -1;
	}
	/**
	 *Judges the command.
	 *@param CmdString the command of the user input  
	 *@param Cmd the handled command 
	 *@throws XylFTPException on dealing with command error
	 *@return true while right,false while wrong
	 */
	private  boolean StartsOnlyWith(String CmdString, String Cmd) throws Exception{
		switch(CountArgs(CmdString)){
		case 1:
			if (CmdString.equalsIgnoreCase(Cmd))
				return true;
			else	
				return false;
		default:
			String []cmds = CmdString.split("[\t ]+");
			if(cmds[0].equalsIgnoreCase(Cmd)) 
				return true;
			else	
				return false;
		}
	}
	
	/**
	 *Translates the command for FTP protocol.
	 *@param Input the input of the user
	 *@return ehco for input
	 *@see Exception
	 *@see XylFTPException
	 *@throws XylFTPException on parsing input error
	 */
	private String ParseInput(String Input) throws Exception{
		String tmp = Input.trim();
		int which = 0;
		if (tmp.equals(""))
			return null;
		String tmps[] = tmp.split("[\t ]+");
		if (tmps!=null)
			which = LookupCommands(tmps[0]);
		else
			which = LookupCommands(tmp);
		switch (which) {
		case 0:
		case 1:
		{
			switch (CountArgs(tmp)){
			case 1:
				ShowHelp("");
				break;
			case 2:
				String []cmds = tmp.split("[\t ]+");
				ShowHelp(cmds[1]);
				break;
			default:
				throw new XylFTPException("help", "Too many arguments.");
			}
			return null;
		}
		case 2:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("ls", "Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				SetLocalFile("");
				if (IsPassive())
					return "PASV\r\nLIST\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nLIST\r\n";
			default:
				String []cmds = tmp.split("[\t ]+");
				String substr = "";
				for(int i=1; i< cmds.length; i++)
					substr = substr + cmds[i] + " ";
				SetLocalFile("");
				substr = substr.trim();
				if (IsPassive())
					return "PASV\r\nLIST "+substr+"\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nLIST "+substr+"\r\n";
			}
		}
		case 3:
		{
			switch (CountArgs(tmp)){
			case 1:
				String[] list = CurrentDir.list();
				for (int i=0; i < list.length; i++) {
					System.out.println(list[i]);
				}
				break;
			default:
				String []cmds = tmp.split("[\t ]+");
				File f2;
				for (int i=1; i< cmds.length; i++) {
					f2 = new File(CurrentDir.getCanonicalPath()+File.separator+cmds[i]);
					if (!f2.exists()) {
						System.out.println(cmds[i]+" doesn't exist.");
						break;
					} else {
						if (f2.isFile())
							System.out.println(cmds[i]);
						if (f2.isDirectory()){
							System.out.println(cmds[i]+":");
							String[] lst = f2.list();
							for(int j=0; j < lst.length; j++)
								System.out.println("\t"+lst[j]);
						}
					}
				}
			}
			return null;
		}
		case 4:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("dir", "Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				SetLocalFile("");
				if (IsPassive())
					return "PASV\r\nNLST\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nNLST\r\n";
			default:
				String []cmds = tmp.split("[\t ]+");
				String substr = "";
				for(int i=1; i< cmds.length; i++)
					substr = substr + cmds[i] + " ";
				SetLocalFile("");
				substr = substr.trim();
				if (IsPassive())
					return "PASV\r\nNLST "+substr+"\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nNLST "+substr+"\r\n";
			}
		}
		case 5:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("get", "You can't excute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				throw new XylFTPException("get", "Missed arguments.");
			case 2:
				String []cmds = tmp.split("[\t ]+");
				String tmp3 = cmds[1];
				if (cmds[1].charAt(0)!='/')
					tmp3 = CurrentDir.getCanonicalPath()+File.separator+cmds[1];
				int p = tmp3.lastIndexOf("/");
				if (p!=-1) {
					File d = new File(tmp3.substring(0, p));
					if (!d.exists())
						throw new XylFTPException("get", "Directory doesn't exist.");
				}
				SetLocalFile(tmp3);	
				File f = new File(tmp3);
				if (f.isDirectory())
					throw new XylFTPException("get", "can't gets to a directory.");
				if (IsPassive())
					return "PASV\r\nRETR "+cmds[1]+"\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nRETR "+cmds[1]+"\r\n";
			case 3:
				String []cmds2 = tmp.split("[\t ]+");
				String tmp4 = cmds2[2];
				if (cmds2[2].charAt(0)!='/')
					tmp4 = CurrentDir.getCanonicalPath()+File.separator+cmds2[2];
				int q = tmp4.lastIndexOf("/");
				if (q!=-1) {
					File d2 = new File(tmp4.substring(0, q));
					if (!d2.exists())
						throw new XylFTPException("get", "Directory doesn't exist.");
				}
				SetLocalFile(tmp4);
				File f2 = new File(tmp4);
				if (f2.isDirectory())
					throw new XylFTPException("get", "can't gets to a directory.");
				if (IsPassive())
					return "PASV\r\nRETR "+cmds2[1]+"\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nRETR "+cmds2[1]+"\r\n";
			default:
				throw new XylFTPException("get", "Too many arguments.");
			}
		}
		case 6:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("put", "You can't excute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				throw new XylFTPException("put", "Missed arguments.");
			case 2:	
				String []cmds = tmp.split("[\t ]+");
				String tmp1 = cmds[1];
				if (cmds[1].charAt(0)!='/')
					tmp1 = CurrentDir.getCanonicalPath()+File.separator+cmds[1];
				SetLocalFile(tmp1);
				File f = new File(tmp1);
				if (!f.exists())
					throw new XylFTPException("put", "file doesn't exist.");
				if (f.isDirectory())
					throw new XylFTPException("put", "can't puts from a directory.");
				if (IsPassive())
					return "PASV\r\nSTOR "+cmds[1]+"\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nSTOR "+cmds[1]+"\r\n";
			case 3:
				String []cmds2 = tmp.split("[\t ]+");
				String tmp2 = cmds2[1];
				if (cmds2[1].charAt(0)!='/')
					tmp2 = CurrentDir.getCanonicalPath()+File.separator+cmds2[1];
				SetLocalFile(tmp2);
				File f2 = new File(tmp2);
				if (!f2.exists())
					throw new XylFTPException("put", "file doesn't exist.");
				if (f2.isDirectory())
					throw new XylFTPException("put", "can't puts from a directory.");
				if (IsPassive())
					return "PASV\r\nSTOR "+cmds2[2]+"\r\n";
				else
					return "PORT "+GetSelfIP()+GetSelfPort()+"\r\nSTOR "+cmds2[2]+"\r\n";
			default:
				throw new XylFTPException("put", "Too many arguments.");
			}
		}
		case 7:
		case 8:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("cd", "You can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				return null;
			case 2:
				String []cmds = tmp.split("[\t ]+");
				return "CWD "+cmds[1]+"\r\n";
			default:
				throw new XylFTPException("cd", "Too many arguments.");
			}
		}
		case 9:
		case 10:
		{
			switch (CountArgs(tmp)){
			case 1:
				break;
			case 2:
				String []cmds = tmp.split("[\t ]+");
				if (cmds[1].equals("."))
					break;
				else if (cmds[1].equals("..")) {
					String parent = CurrentDir.getAbsoluteFile().getParent();
					if (parent==null) {
						break;
					} else {
						System.out.println("cd into: "+parent);
						CurrentDir = new File(parent);
					}
				} else if (cmds[1].charAt(0)=='/') {
					File ff = new File(cmds[1]);
					if(!ff.exists() || !ff.isDirectory())
						System.out.println(cmds[1]+": No such dir.");
					else {
						CurrentDir = ff;
						System.out.println("cd into: "+CurrentDir.getCanonicalPath());
					}

				} else{
					File fl = new File(CurrentDir.getCanonicalPath()+File.separator+cmds[1]);
					if(!fl.exists() || !fl.isDirectory())
						System.out.println(cmds[1]+": No such dir.");
					else {
						CurrentDir = fl;
						System.out.println("cd into: "+CurrentDir.getCanonicalPath());
					}
				}
				break;
			default:
				throw new XylFTPException("cd", "Too many arguments.");
			}
			return null;
		}
		case 11:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("pwd", "You can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				return "PWD\r\n";
			default:
				throw new XylFTPException("pwd", "It doesn't accept any arguments.");
			}
		}
		case 12:
		{
			switch (CountArgs(tmp)){
			case 1:
				System.out.println(CurrentDir.getCanonicalPath());
				break;
			default:
				throw new XylFTPException("lpwd", "It doesn't accept any arguments.");
			}
			return null;
		}
		case 13:
		{
			switch (CountArgs(tmp)){
			case 2:
				String []cmds = tmp.split("[\t ]+");
				if (!cmds[1].equals("on") && !cmds[1].equals("off"))
					throw new XylFTPException("passive", "Wrong arugment.");
				else {
					if (cmds[1].equals("on")) {
						System.out.println("Passive mode on.");
						SetPassive();
					} else {
						System.out.println("Passive mode off.");
						SetActive();
					}
				}
				break;
			case 1:
				throw new XylFTPException("passive", "It must have an argument.");
			default:
				throw new XylFTPException("passive", "Too many arguments.");
			}
			return null;
		}
		case 14:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("cdup",
					"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				return "CDUP\r\n";
			default:
				throw new XylFTPException("cdup",
					"It doesn't accept any arguments.");
			}
		}
		case 15:
		{
			switch (CountArgs(tmp)){
			case 1:
				String parent = CurrentDir.getAbsoluteFile().getParent();
				if (parent==null) {
					break;
				} else {
					System.out.println("cd into: "+parent);
					CurrentDir = new File(parent);
				}
				break;
			default:
				throw new XylFTPException("lcdup",
					"It doesn't accept any arguments.");
			}
			return null;
		}
		case 16:
		case 17:
		{
			if (CountArgs(tmp) > 1) {
				throw new XylFTPException("quit",
					"It doesn't accept any arguments.");
			}
			switch (GetStatus()){
			case 0:
				System.exit(0);
			case 1:
			case 2:
				return "QUIT\r\nQUIT\r\n";	//Note! It is used to differ 'quit' from 'close'.
			case 3:
			case 4:
				return "ABOR\r\nQUIT\r\nQUIT\r\n";
			default:
				throw new XylFTPException("Unknown status!");
			}
		}
		case 18:
		{
			int n = CountArgs(tmp);
			if (n < 2){
				throw new XylFTPException("open",
					"It must be followed by at least one argument.");
			}
			if (n > 3){
				throw new XylFTPException("open", "Too many arguments.");
			}
			switch (GetStatus()){
			case 0:
			case 1:
				int portNum;
				String []cmds = tmp.split("[\t ]+");
				if(cmds.length==2) {
					portNum = 21;
				} else {
					try {
						portNum = Integer.parseInt(cmds[2]);
					} catch (NumberFormatException e) {
						ShowHelp("open");
						return null;
					}
				}
				SetHost(cmds[1]);
				SetPort(portNum);
				OpenConnection();
				String s = GetEcho();
				if (s==null)
					throw new XylFTPException("xylftp", 0,
								"Cann't get an echo.");
				else
					System.out.println(s);
				while (s.charAt(3) == '-'){
					s = GetEcho();
					if (s==null)
						throw new XylFTPException("xylftp", 0,
								"Can't get an echo.");
					if (XylFTPMain.GetEnableDebug())
						System.out.println("<---"+s);
					System.out.println(s);
				}
				SetStatus(1);
				return "USER "+UserName+"\r\n"+"PASS "+PassWord+"\r\n";
			case 2:
			case 3:
			case 4:
				throw new XylFTPException("Connection already existed.");
			default:
				throw new XylFTPException("panic", "Unknown status!");
			}
		}
		case 19:
		{
			if (CountArgs(tmp) > 1){
				throw new XylFTPException("close", "Too many arguments.");
			}
			switch (GetStatus()){
			case 0:
				throw new XylFTPException("Not connected yet.");
			case 1:
			case 2:
				return "QUIT\r\n";
			case 3:
			case 4:
				return "ABOR\r\nQUIT\r\n";
			default:
				throw new XylFTPException("Unknown status!");
			}
		}
		case 20:
		{
			switch (GetStatus()){
			case 0:
				String [] cmds = tmp.split("[\t ]+");
				if (cmds.length == 2) {
					UserName =  cmds[1];
				}
				else if (cmds.length == 3) {
					UserName =  cmds[1];
					PassWord = cmds[2];
				}
				else if (cmds.length == 1) {
					throw new XylFTPException("user",
						"It must be followed by at least one argument.");
				}
				else {
					throw new XylFTPException("user", "Too many arguments.");
				}
				SetUserName(UserName);
				SetPassWord(PassWord);
				return null;
			case 1:
			case 2:
				String [] cmds2 = tmp.split("[\t ]+");
				if (cmds2.length == 2) {
					UserName =  cmds2[1];
				}
				else if (cmds2.length == 3) {
					UserName =  cmds2[1];
					PassWord = cmds2[2];
				}
				else if (cmds2.length == 1) {
					throw new XylFTPException("user",
						"It must be followed by at least one argument.");
				}
				else{
					throw new XylFTPException("user", "Too many arguments.");
				}
				return "USER "+UserName+"\r\n"+"PASS "+PassWord+"\r\n";
			case 3:
			case 4:
				throw new XylFTPException("user",
						"Can't execute it now. Try again later.");
			default:
				throw new XylFTPException("panic", "Unknown status!");
			}
		}
		case 21:
		{
			if (CountArgs(tmp) > 1) {
				throw new XylFTPException("!!", "It doesn't accept any arguments.");
			} else {
				System.out.println("=====Enter shell mode=====");
				String cmd = GetInput();
				System.out.println("cmd :"+cmd);
				while(!cmd.equals("exit")){
					Runtime run = Runtime.getRuntime();
					Process pp=run.exec(cmd);
					pp.waitFor();
					BufferedReader in = new BufferedReader(new InputStreamReader(pp.getInputStream()));
					String line;
					while ((line = in.readLine()) != null) {
						System.out.println(line);
					}
					cmd = GetInput();
				}
				System.out.println("=====Exit shell mode=====");
				return null;
			}
		}
		case 22:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("delete",
						"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				throw new XylFTPException("delete", "Missed arguments.");
			case 2:
				String []cmds = tmp.split("[\t ]+");
				return "DELE "+cmds[1]+"\r\n";
			default:
				throw new XylFTPException("delete", "Too many arguments.");
			}
		}
		case 23:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("rmdir",
						"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				throw new XylFTPException("rmdir", "Missed arguments.");
			case 2:
				String []cmds = tmp.split("[\t ]+");
				return "RMD "+cmds[1]+"\r\n";
			default:
				throw new XylFTPException("rmdir", "Too many arguments.");
			}
		}
		case 24:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("mkdir",
						"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				throw new XylFTPException("mkdir", "Missed arguments.");
			case 2:
				String []cmds = tmp.split("[\t ]+");
				return "MKD "+cmds[1]+"\r\n";
			default:
				throw new XylFTPException("mkdir", "Too many arguments.");
			}
		}
		case 25:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("chmod",
						"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
			case 2:
				throw new XylFTPException("chmod", "Missed arguments.");
			case 3:
				String []cmds = tmp.split("[\t ]+");
				return "SITE CHMOD "+cmds[1]+" "+cmds[2]+"\r\n";
			default:
				throw new XylFTPException("chmod", "Too many arguments.");
			}
		}
		case 26:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("size",
						"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
				throw new XylFTPException("size", "Missed arguments.");
			case 2:
				String []cmds = tmp.split("[\t ]+");
				return "SIZE "+cmds[1]+"\r\n";
			default:
				throw new XylFTPException("size", "Too many arguments.");
			}
		}
		case 27:
		{
			if (GetStatus()!=2)
				throw new XylFTPException("rename",
						"Can't execute it now. Try again later.");
			switch (CountArgs(tmp)){
			case 1:
			case 2:
				throw new XylFTPException("rename", "Missed arguments.");
			case 3:
				String []cmds = tmp.split("[\t ]+");
				return "RNFR "+cmds[1]+"\r\nRNTO "+cmds[2]+"\r\n";
			default:
				throw new XylFTPException("rename", "Too many arguments.");
			}

		}
		case 28:
		{
			switch (CountArgs(tmp)){
			case 1:
				if(GetTransferMode()==0)
					System.out.println("Using binary mode to transfer files.");
				else
					System.out.println("Using ascii mode to transfer files.");
				return null;
			case 2:
				String []cmds = tmp.split("[\t ]+");
				if(cmds[1].equalsIgnoreCase("ascii"))
					return "TYPE A\r\n";
				else if(cmds[1].equalsIgnoreCase("binary"))
					return "TYPE I\r\n";
				else
					throw new XylFTPException("type", "Wrong arguments.");
			default:
				throw new XylFTPException("type", "Too many arguments.");
			}
			
		}
		case 29:
		{ 
			switch (GetStatus()){
			case 0: 
				System.out.println("Not connected.");
				break;
			case 1:
				System.out.println("Connected to "+GetHost()+",but not login.");
				break;
			case 2:
				System.out.println("Login ("+GetHost()+") and no data transfer.");
				break;
			case 3:
				System.out.println("Login ("+GetHost()+") and getting data down.");
				break;
			case 4:
				System.out.println("Login ("+GetHost()+") and putting data up.");
				break;
			default:
				throw new XylFTPException("status", "Wrong status.");
			}

			if (IsPassive()){
				System.out.println("Passive: on");
			}
			else
				System.out.println("Passive: off");

			if (GetTransferMode()==0)
				System.out.println("Type: binary");
			else
				System.out.println("Type: ascii");


			if (XylFTPMain.GetEnableVerbose()){
				System.out.println("Verbose: on");
			}
			else
				System.out.println("Verbose: off");

			if (XylFTPMain.GetEnableDebug()){
				System.out.println("Debug: on");
			}
			else
				System.out.println("Debug: off");

			return null;
		}
		case 30:
		{
			String in, echo;
			int ret;
			if (GetStatus()==0)
				throw new XylFTPException("quote", "Not connection.");
			switch(CountArgs(tmp)){
			case 1:
				System.out.print("Enter the command to send:");
				in = GetInput();
				break;
			default:
				in = tmp.substring(5, tmp.length()).trim();
			}
			SendCommand(in);
			if (XylFTPMain.GetEnableDebug())
				System.out.println("--->" + in);
			do {
				echo = GetEcho();
				if(echo == null || !IsValidEcho(echo))
					throw new XylFTPException("Can't get an echo.");
				if (XylFTPMain.GetEnableDebug())
					System.out.println("<---" + echo);
				ret = ProcessEcho(echo);
			} while(ret == 6);
			return null;
		}
		case 31:
		{
			switch(CountArgs(tmp)){
			case 1:
				if (XylFTPMain.GetEnableVerbose()){
					XylFTPMain.SetEnableVerbose(false);
					System.out.println("Verbose off.");
				}
				else { 
					XylFTPMain.SetEnableVerbose(true);
					System.out.println("Verbose on.");
				}
				break;
			default:
				throw new XylFTPException("verbose", "Too many arguments.");
			}
			return null;
		}
		case 32:
		{
			switch(CountArgs(tmp)){
			case 1:
				if (XylFTPMain.GetEnableDebug()){
					XylFTPMain.SetEnableDebug(false);
					System.out.println("Debugging off.");
				}
				else {
					XylFTPMain.SetEnableDebug(true);
					System.out.println("Debugging on.");
				}
				break;
			default:
				throw new XylFTPException("debug", "Too many arguments.");
			}
			return null;
		}
		default:
			throw new XylFTPException("xylftp", GetStatus(), "Unknown command!");
		}
	}
	
	/**
	 *Reads the input.
	 *@return In the readline from buffer
	 *@see Exception 
	 *@throws Exception
	 */
	public String GetInput() throws Exception{
		//Why System.in.skip(System.in.available()); cann't help us?
		BufferedReader Buf = new BufferedReader(new InputStreamReader(System.in));
		String In = Buf.readLine();
		return In;
	}
	
	/**
	 *Shows the echo from server.
	 *@param Echo 
	 */
	private void ShowEcho(String Echo){
		System.out.println(Echo);
	}

	/**
	 *Determines whether the echo is valid
	 *@param OEcho
	 */
	public boolean IsValidEcho(String OEcho){
		String Echo = OEcho.trim();
		if (Echo.length() < 4)
			return false;
		if (Echo.charAt(0)<'0' || Echo.charAt(0)>'9')
			return false;
		if (Echo.charAt(1)<'0' || Echo.charAt(1)>'9')
			return false;
		if (Echo.charAt(2)<'0' || Echo.charAt(2)>'9')
			return false;
		if (Echo.charAt(3)!=' ' && Echo.charAt(3)!='-')
			return false;
		return true;
	}
	
	/**
	 *Process the echo from the server.
	 *@param OEcho a number returns from the server
	 *@return a number that shows the echo as a logo
	 */
	public int ProcessEcho(String OEcho){
		String Echo = OEcho.trim();
		ShowEcho(Echo);
		if (Echo.charAt(3) == '-') 
			return 6;	//This means multi-line echo.
		if(Echo.substring(0,1).equals("1"))
			return 1;	//We need data connection.
		if(Echo.substring(0,1).equals("2")){
			if(Echo.substring(0,3).equals("227"))
				return 3;
			else if(Echo.substring(0,3).equals("200"))
				return 3;
			else
				return 2;	//Don't to be continued.
		}
		if(Echo.substring(0,1).equals("3"))
			return 3;	//Continue.
		if(Echo.substring (0,1).equals("4")){
			if(Echo.substring(0,3).equals("421")){
				SetStatus(0);
				return 2;
			}
			if(Echo.substring(0,3).equals("450"))
				return 2;
			return 4;	//Send again.
		}
		if(Echo.substring(0,1).equals("5"))
			return 5;	//Stop resending.
		if(Echo.substring(1,2).equals("0"))
			return 0;	//Invalid format.
		else
			return -1;	//Unknow mistake
	}
	
	/**
	 *Gets the commands that user inputs.
	 *@throws XylFTPException on getting commands error
	 *@see XylFTPException
	 *@return the handled command as a string  
	 */
	public String[] GetCommands() throws Exception{
		String CmdString = ParseInput(GetInput());
		String[] cmds;
		cmds = MakeCommands(CmdString);
		return cmds;
	}
	
	/**
	 *Show the usage.
	 */
	public void ShowUsage(){
		System.out.println("xylftp [-h|--help][-V | -version]");
		System.out.println("xylftp [-v|--verbose][-u $USERNAME|--user=$USERNAME][-p $PASSWORD | --password=$PASSWORD][-d|--debug] [$HOST]");
		System.out.println("");
		System.out.println("-u $username | -user=$username \n\tTry $username as username to connect to the host, if this is not specified, use anonymous as default.");
		System.out.println("-h | --help \n\tShow this help screen.");
		System.out.println("-V | -version \n\tShow version information.");
		System.out.println("-v | -verbose \n\tShow verbose information.");
		System.out.println("-p $PASSWORD | --password=$PASSWORD \n\tUse $PASSWORD as your password. If this is unspecified, use empty password as default.");
		System.out.println("-d | --debug \n\tShow more information for developers and system admins.");
	}
	
	/**
	 *Show help information.
	 */
	public void ShowHelp(String Command){
		String help[] = new String[31];
		int i = 0;

		help[0]="?\tprint local help information";
		help[1]="bye\tterminate ftp session and exit";
		help[2]="cd\tchange remote working directory";
		help[3]="cdup\tchange remote working directory to parent directory";
		help[4]="chmod\tchange file permissions of remote file";
		help[5]="close\tterminate ftp session";
		help[6]="debug\toggle/set debugging mode";
		help[7]="delete\tdelete remote file";
		help[8]="dir\tlist contents of remote directory";
		help[9]="get\tget a file";
		help[10]="help\tprint local help information";
		help[11]="lcd\tchange local working directory";
		help[12]="lcdup\tget to local parent directory";
		help[13]="lcwd\tchange local working directory";
		help[14]="lls\tlist contents of local directory";
		help[15]="lpwd\tprint working directory on local machine";
		help[16]="ls\tlist contents of remote directory";
		help[17]="mkdir\tmake directory on the remote machine";
		help[18]="open\tconnect to remote ftp";
		help[19]="passive\tturn passive transfer mode on/off";
		help[20]="put\tsend one file";
		help[21]="pwd\tprint working directory on remote machine";
		help[22]="quit\tterminate ftp session and exit";
		help[23]="quote\tsend arbitrary ftp command";
		help[24]="rename\trename a file";
		help[25]="rmdir\tremove directory on the remote machine";
		help[26]="size\tshow size of remote file";
		help[27]="status\tshow current status";
		help[28]="type\tset file transfer type";
		help[29]="user\tsend new user information";
		help[30]="verbose\ttoggle verbose mode";

		if (Command.equals("")){
			System.out.println("Help of xylftp commands:\n");
			while(i < help.length){
				System.out.println(help[i]);
				i++;
			}
		} else {
			while(i < help.length){
				if(help[i].startsWith(Command+"\t")){
					System.out.println(help[i]);
					return ;			
				}
				i++;
			}
			System.out.println("There is no information about "+Command);
		}
	}
	
	/**
	 *Show the version.
	 */
	public void ShowVersion(){
		System.out.println("xylftp: version 1.1.");
	}

}
