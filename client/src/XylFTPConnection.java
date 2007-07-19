//////////////////////////////////////////////////////////////////////////
// Copyright (C) Zhou Xiao-wei, Apr. 2007.
// Copyright (C) Wang Cong, Apr. 2007.
// See AUTHORS and CREDITS to learn more.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////

import java.net.*;
import java.io.*;

/**
 * This class is for FTP connection.
 * Extends XylTelnetConnection.
 * @author WANG Cong, Zhou Xiao-Wei
 * @version 1.14
 */
public class XylFTPConnection extends XylTelnetConnection{
	
	/**
	 *The size of reader's buffer.
	 */
	private final int RDBUF_SIZE = 1024;
	
	/**
	 *The size of writer's buffer.
	 */
	private final int WRBUF_SIZE = 1024;
	
	/**
	 *The status of FTP connection.
	 */
	private int Status;	
	// 0 for not connected
	// 1 for connected but not login
	// 2 for login and no data transfer
	// 3 for login and getting data down
	// 4 for login and putting data up

	/**
	 *Presents the mode (active or passive) of FTP connection.
	 */
	private int Mode;	
	//1 for passive, 0 for active, defaults to 1. ;)

	/**
	 *Gets the line separator used by local system.
	 */
	private byte[] LineSeparator = System.getProperty("line.separator").getBytes();

	/**
	 *Presents the mode (ascii or binary) of transfer files.
	*/
	private int TransferMode;
	//1 for ascii, 0 for binary, defaults to 0. ;)

	/**
	 *Constructs a LocalFile for data storage.
	 */
	private String LocalFile;
	// Empty string for this means stdout. ;)

	/**
	 *The data connection socket.
	 */
	private Socket DataSocket;
	
	/**
	 *The sever data socket, used by active mode.
	 */
	private ServerSocket DataServerSocket;
	
	/**
	 *The input stream. 
	 */
	private BufferedInputStream FTPInStream;
	
	/**
	 *The output stream.
	 */
	private BufferedOutputStream FTPOutStream;

	/**
	 *Initializes some values.
	 */
	XylFTPConnection(){
		super();
		Status = 0;
		Mode = 1;
		TransferMode = 0;
		LocalFile = null;
		DataSocket = null;
		DataServerSocket = null;
	}
	
	/**
	 *Gets status of current FTP connection.
	 *@return the status of connection
	 */
	public int GetStatus(){
		return Status;
	}
	
	/**
	 *Sets status of the connection.
	 *@param NewStatus 
	 */
	public void SetStatus(int NewStatus){
		Status = NewStatus;
	}
	
	/**
	 *Get Mode of tranfer files.
	 *@return the mode of transfer files
	*/
	public int GetTransferMode(){
		return TransferMode;
	}

	/**
	 *Sets TransferMode of transfer files.
	 *@param NewTransferMode New Transfer Mode
	*/
	public void SetTransferMode(int NewTransferMode){
		TransferMode = NewTransferMode;
	}

	/**
	 *Sets the LocalFile name.
	 *@param FileName
	 */
	public void SetLocalFile(String FileName){
		LocalFile = FileName;
	}
	
	/**
	 *Tests whether the mode is passive
	 */
	public boolean IsPassive(){
		if (Mode==1)
			return true;
		else
			return false;
	}
	/**
	 *Sets the passive mode.
	 */
	public void SetPassive(){
		Mode = 1;
	}
	
	/**
	 *Sets the active mode.
	 */
	public void SetActive(){
		Mode = 0;
	}

	/**
	 *Prepares for active data connection.
	 */
	public void ReadyForActive() throws Exception{
		try {
			if(-1 == ReturnSelfPort())
				throw new XylFTPException("Port is unavailable.");
			DataServerSocket = new ServerSocket(ReturnSelfPort());
			DataServerSocket.setSoTimeout(Timeout/2);//Hmm, short is better.
		} catch(Exception e) {
			SetStatus(2);
			throw e;
		}
	}
	/**
	 *Opens the data connection in active mode.
	 *@see Exception
	 *@throws Exception on opening dataconnection error
	 */
	public void OpenDataConnection() throws Exception{
		try {
			DataSocket = DataServerSocket.accept();
		} catch(Exception e) {
			SetStatus(2);
			throw e;
		}
	}
	
	/**
	 *Opens the data connection in passive mode.
	 *@param Port the port of server
	 *@param Host the IP of server
	 *@see Exception
	 *@throws Exception on opening DataConnection error
	 */
	public void OpenDataConnection(String Host, int Port) throws Exception{
		try {
			DataSocket = new Socket(Host, Port);
			DataSocket.setSoTimeout(Timeout);
		} catch (Exception e) {
			SetStatus(2);
			throw e;
		}
	}
	
	/**
	 *Closes the data connection.
	 *@see XylFTPException
	 *@throws XylFTPException on closing DataConnection error
	 */
	public void CloseDataConnection() throws Exception{
		DataSocket.close();
		if(DataServerSocket != null) {
			DataServerSocket.close();
		}
	}
	
	/**
	 *Uploads a file in active mode.
	 *@throws XylFTPException on sending file error
	 *@see XylFTPException
	 */
	public void SendFileActive() throws Exception{
		int bytesRead;
		byte [] buf = new byte[WRBUF_SIZE];
		if (Mode!=0) {
			SetStatus(2);
			throw new XylFTPException("You are not in active mode!");
		}
		if (LocalFile.equals("")) {
			SetStatus(2);
			throw new XylFTPException("Can't send unnamed file!");
		} else {
			FTPInStream = new BufferedInputStream(new FileInputStream(LocalFile));
		}
		FTPOutStream = new BufferedOutputStream(DataSocket.getOutputStream());

		if (TransferMode==1) {
			int separatorPos = 0;
			while ((bytesRead = FTPInStream.read(buf, 0, WRBUF_SIZE)) > 0) {
				for (int i = 0; i< bytesRead; i++) {
					boolean found = true;
					int skip = 0;
					for (; separatorPos < LineSeparator.length
						&& i+separatorPos < bytesRead;
						skip++, separatorPos++) {
						if (buf[i+separatorPos] != LineSeparator[separatorPos]) {
							found = false;
							break;
						}//if
					}//for
					if (found) { // either found match or run out of buffer
						if (separatorPos == LineSeparator.length) {
							// found line separator
							FTPOutStream.write('\r');
							FTPOutStream.write('\n');
							separatorPos = 0;
							//skip over bytes that match
							i += (skip-1);
						} else {
							// reached end of buffer && matching so far
							// Do nothing. ;)
						}//else
					} else{
						FTPOutStream.write(buf[i]);
					}
				}//for
			}//while
		} else if (TransferMode==0) {
			while ((bytesRead = FTPInStream.read(buf, 0, WRBUF_SIZE)) > 0) {
				FTPOutStream.write(buf, 0, bytesRead);
			}
		} else {
			throw new XylFTPException("panic", 0, "No such mode!");
		}
		FTPOutStream.flush();
		FTPOutStream.close();
		FTPInStream.close();
	}
	/**
	 *Uploads a file in passive mode.
	 *@throws XylFTPException on sending file error
	 *@see XylFTPException
	 */
	public void SendFilePassive() throws Exception{
		int bytesRead;
		byte [] buf = new byte[WRBUF_SIZE];
		if (Mode!=1) {
			SetStatus(2);
			throw new XylFTPException("You are not in passive mode!");
		}
		if (LocalFile.equals("")) {
			SetStatus(2);
			throw new XylFTPException("Can't send unnamed file!");
		} else {
			FTPInStream = new BufferedInputStream(new FileInputStream(LocalFile));
		}
		FTPOutStream = new BufferedOutputStream(DataSocket.getOutputStream());
		if (TransferMode==1) {
			int separatorPos = 0;
			while ((bytesRead = FTPInStream.read(buf, 0, WRBUF_SIZE)) > 0) {
				for (int i = 0; i< bytesRead; i++) {
					boolean found = true;
					int skip = 0;
					for (; separatorPos < LineSeparator.length
						&& i+separatorPos < bytesRead;
						skip++, separatorPos++) {
						if (buf[i+separatorPos] != LineSeparator[separatorPos]) {
							found = false;
							break;
						}//if
					}//for
					if (found) { // either found match or run out of buffer
						if (separatorPos == LineSeparator.length) {
							// found line separator
							FTPOutStream.write('\r');
							FTPOutStream.write('\n');
							separatorPos = 0;
							//skip over bytes that match
							i += (skip-1);
						} else {
							// reached end of buffer && matching so far
							// Do nothing. ;)
						}//else
					} else{
						FTPOutStream.write(buf[i]);
					}
				}//for
			}//while
		} else if (TransferMode==0) {
			while ((bytesRead = FTPInStream.read(buf, 0, WRBUF_SIZE)) > 0) {
				FTPOutStream.write(buf, 0, bytesRead);
			}
		} else {
			throw new XylFTPException("panic", 0, "No such mode!");
		}
		FTPOutStream.flush();
		FTPOutStream.close();
		FTPInStream.close();
	}
	
	/**
	 *Downloads a file in active mode.
	 *@throws XylFTPException on getting file error
	 *@see XylFTPException
	 */
	public void GetFileActive() throws Exception{
		int bytesRead;
		byte [] buf = new byte[RDBUF_SIZE];
		if (Mode!=0) {
			SetStatus(2);
			throw new XylFTPException("You are not in active mode!");
		}
		if (LocalFile.equals("")) {
			FTPOutStream = new BufferedOutputStream((OutputStream)System.out);
		} else {
			FTPOutStream = new BufferedOutputStream(new FileOutputStream(LocalFile));
		}
		FTPInStream = new BufferedInputStream(DataSocket.getInputStream());

		if (TransferMode==1) {
			boolean crFound = false;
			while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
				boolean lfFound = false;
				for (int i = 0; i < bytesRead; i++) {
		                        lfFound = buf[i] == '\n';
					if (crFound) {
						if (lfFound) {
							FTPOutStream.write(LineSeparator, 0,
								LineSeparator.length);
						} else {
							FTPOutStream.write('\r');
						}                           
					}
                        		crFound = buf[i] == '\r';
                        		if (!lfFound && !crFound) {
						FTPOutStream.write(buf[i]);
					}//if
				}//for
			}//while
		} else if (TransferMode==0) {
			while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
				FTPOutStream.write(buf, 0, bytesRead);
			}
		} else {
			throw new XylFTPException("panic", 0, "No such mode!");
		}
		FTPOutStream.flush();
		if (!LocalFile.equals(""))	//We can NOT close the stdout.
			FTPOutStream.close();
		FTPInStream.close();
	}
	
	/**
	 *Downloads a file in passive mode.
	 *@throws XylFTPException on getting file error
	 *@see XylFTPException
	 */
	public void GetFilePassive() throws Exception{
		int bytesRead;
		byte [] buf = new byte[RDBUF_SIZE];
		if (Mode!=1) {
			SetStatus(2);
			throw new XylFTPException("You are not in passive mode!");
		}
		if (LocalFile.equals("")) {
			FTPOutStream = new BufferedOutputStream((OutputStream)System.out);
		} else {
			FTPOutStream = new BufferedOutputStream(new FileOutputStream(LocalFile));
		}
		FTPInStream = new BufferedInputStream(DataSocket.getInputStream());
		if (TransferMode==1) {
			boolean crFound = false;
			while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
				boolean lfFound = false;
				for (int i = 0; i < bytesRead; i++) {
		                        lfFound = buf[i] == '\n';
					if (crFound) {
						if (lfFound) {
							FTPOutStream.write(LineSeparator, 0,
								LineSeparator.length);
						} else {
							FTPOutStream.write('\r');
						}                           
					}
                        		crFound = buf[i] == '\r';
                        		if (!lfFound && !crFound) {
						FTPOutStream.write(buf[i]);
					}//if
				}//for
			}//while
		} else if (TransferMode==0) {
			while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
				FTPOutStream.write(buf, 0, bytesRead);
			}
		} else {
			throw new XylFTPException("panic", 0, "No such mode!");
		}
		FTPOutStream.flush();
		if (!LocalFile.equals(""))	//We can NOT close the stdout.
			FTPOutStream.close();
		FTPInStream.close();
	}
}

