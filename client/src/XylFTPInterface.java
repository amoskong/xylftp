//////////////////////////////////////////////////////////////////////////
// Copyright (C) Wang Cong, Apr. 2007.
// See AUTHORS and CREDITS to learn more.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////

/**
 *This interface is for both CLI and GUI.
 *@author WANG Cong
 *@version 1.5
 */
public interface XylFTPInterface{
	
	/**
	 *Processes the echos from the server.
	 *@param Echo the echo from the server
	 */
	public abstract int ProcessEcho(String Echo);
	
	/**
	 *Gets FTP commands from the user.
	 *@exception XylFTPException on getting commands error
	 *@see XylFTPException
	 */
	public abstract String[] GetCommands() throws Exception;
}
