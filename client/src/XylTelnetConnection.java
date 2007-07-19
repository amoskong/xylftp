//////////////////////////////////////////////////////////////////////////
// Copyright (C) Wang Cong, Apr. 2007.
// Hacked by Zhou Xiao-Wei.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////
import java.net.*;
import java.io.*;

/**
 *This class is a subclass of XylFTPConnection.
 *@author WANG Cong
 *@version 1.7
 */
public class XylTelnetConnection{
	
	/**
	 *The hostname of the server to connect to.
	 */
 	private String Host;
 	
	/**
	 *The port to connect to
	 */	
	private int ServerPort;

	/**
	 *The port of local host
	 */

	private int SelfPort;
	/**
	 *The timeout when attempting to connect a socket.
	 */
 	protected int Timeout;	//in milliseconds
 	
	/**
	 *The writer who inputs the commands.
	 */
 	private Writer CmdWriter;
 	
	/**
	 *The reader that reads the echos from the sever.
	 */
 	private BufferedReader EchoReader;
 	
	/**
	 *The socket of the command needs to start.
	 */
 	private Socket CmdSocket;
 
	/**
	 *Initializes some values.
	 */
	XylTelnetConnection(){
		CmdWriter = null;
		EchoReader = null;
		CmdSocket = null;
		Host = null;
		ServerPort = 21;
		SelfPort = -1;
		Timeout = 30000;	//I don't know whether this is proper. 8(
	}
	
	/**
	 *Initializes some values.
	 *@param HostName the hostname of the server to connect to
	 */
	XylTelnetConnection(String HostName){
		CmdWriter = null;
		EchoReader = null;
		CmdSocket = null;
		Host = HostName;
		ServerPort = 21;
		SelfPort = -1;
		Timeout = 30000;
	}
	
	/**
	 *Gets command from the user.
	 *@param Command the command that user inputs
	 *@exception IOException on sending command error
	 *@see IOException
	 */
	public void SendCommand(String Command) throws IOException{
		try{
			CmdWriter.write(Command+"\r\n");
			CmdWriter.flush();
		} catch (IOException e) {
			throw e;
		}
	}
	/**
	 *Gets the string of the IP address of localhost
	 *Each byte of IP is separated by a _comma_, since
	 *this is for PORT command, _not_ for any socket.
	 */
	public String GetSelfIP(){
		InetAddress ina;
		byte[] ipa;
		String ip = "";
		ina = CmdSocket.getLocalAddress();
		ipa = ina.getAddress();
		for (int i=0; i< 4; i++) {
			ip = ip + (((int)ipa[i])&0xff) + ",";	//I know we have 4 commas!
		}
		return ip;
	}
	/**
	 *Attempts to get an available port of local host for use.
	 */
	public String GetSelfPort() throws IOException {
		ServerSocket sk = new ServerSocket(0);
		int port = sk.getLocalPort();
		SelfPort = port;
		int hi, low;
		low = port%256;
		hi = port/256;
		sk.close();
		return hi+","+low;
	}
	/**
	 *Just returns the port to be used by local host.
	 */
	public int ReturnSelfPort(){
		return SelfPort;
	}
	/**
	 *Tests whether the echo is available.
	 *@return true when available, otherwise false
	 *@exception IOException on error
	 */
	public boolean HasEcho() throws IOException{
		return EchoReader.ready();
	}
	/**
	 *Gets echoes from the server.
	 *@return line the echos from the server
	 *@exception IOException on read error
	 */
 	public String GetEcho() throws IOException{
		String line = EchoReader.readLine();
		while (line != null && line.length() == 0)
			line = EchoReader.readLine();
		return line;
	}	
	
	/**
	 *Starts to connect with the server.
	 *@exception Exception on opening connection error
	 *@see Exception
	 */
 	public void OpenConnection() throws Exception{
		if(Host==null)
			throw new Exception("Unspecified host name.");
		CmdSocket = new Socket(Host, ServerPort);
		CmdSocket.setSoTimeout(Timeout);
		InitStreams();
	}
	
	/**
	 *Initilizes the streams that input and output.
	 *@exception Exception on errors when initializing streams.
	 *@see Exception
	 */
 	private void InitStreams() throws Exception{
		InputStream is = CmdSocket.getInputStream();
		EchoReader = new BufferedReader(new InputStreamReader(is));
		OutputStream os = CmdSocket.getOutputStream();
		CmdWriter = new OutputStreamWriter(os);	
	}

	/**
	 *Closes the connection with the server.
	 *@exception Exception on closing connection error
	 *@see Exception
	 */
 	public void CloseConnection() throws Exception{
		CmdWriter.close();
		EchoReader.close();
		CmdSocket.close();
	}

	/**
	 *Sets a new host.
	 *@param NewHost a new founded host  
	 */
 	public void SetHost(String NewHost) {
		Host = NewHost;
	}

	/**
	 *Gets the host name .
	 *@return the hostname of the server
	 */
 	public String GetHost(){
		return Host;
	}

	/**
	 *Sets a new port.
	 *@param NewPort a new founded port 
	 */
 	public void SetPort(int NewPort) {
		ServerPort = NewPort;
	}

	/**
	 *Gets the port connected with the server.
	 *@return the number of the current port
	 */
 	public int GetPort(){
		return ServerPort;
	}
	
}
