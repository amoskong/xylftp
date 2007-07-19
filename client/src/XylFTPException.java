//////////////////////////////////////////////////////////////////////////
// Copyright (C) Wang Cong, May. 2007.
// See AUTHORS and CREDITS to learn more.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////

/**
 *Deals with the exceptions.
 *Extends Exception.
 *@author  WANG Cong
 *@version 1.5
 */
public class XylFTPException extends Exception{
	
	/**
	 *Command being executed when errors occur.
	 */
	private String Command;

	/**
	 *The status before errors occur.
	 */
	private int PrevStatus;
	 
	/**
	 *A constructor.
	 *@param Msg the message
	 */
	XylFTPException(String Msg){
		super(Msg);
		Command = null;
		PrevStatus = -1;
	}
	
	/**
	 *Also a constructor.
	 *@param Cmd the command 
	 *@param Msg the message
	 */
	XylFTPException(String Cmd, String Msg){
		super(Msg);
		this.Command = Cmd;
		this.PrevStatus = -1;
	}

	/**
	 *Another a constructor.
	 *@param Cmd the command.
	 *@param Status the status before errors occur.
	 *@param Msg the message that describes the error.
	 */
	XylFTPException(String Cmd, int Status, String Msg){
		super(Msg);
		this.Command = Cmd;
		this.PrevStatus = Status;
	}
	
	/**
	 *Gets the message. 
	 *@return the string of message
	 */
	public String GetMessage(){
		if (Command!=null)
			return Command+": "+getMessage();
		else
			return getMessage();
	}
	
	/**
	 *Gets command related to the error.
	 *@return the string of command
	 */
	public String GetCommand(){
		return Command;
	}

	/**
	 *Gets the status before the errors occur.
	 *@return the status
	 */
	public int GetStatus(){
		return PrevStatus;
	}
}

