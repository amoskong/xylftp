//////////////////////////////////////////////////////////////////////////
// Copyright (C) Zhou Xiao-wei, Apr. 2007.
// Copyright (C) Wang Cong, Apr. 2007.
// See AUTHORS and CREDITS to learn more.
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
// See COPYING to learn more.
//////////////////////////////////////////////////////////////////////////

import java.awt.Container;
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
    //1 for passive, 0 for active, default to 1. ;)
    
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
    
    public void ListenForDataConnection() throws Exception{
        try {
            if(-1 == ReturnSelfPort())
                throw new XylFTPException("Port is unavailable.");
            DataServerSocket = new ServerSocket(ReturnSelfPort());
            DataServerSocket.setSoTimeout(Timeout);
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
        if(DataSocket != null) {
            DataSocket.close();
            DataSocket = null;
        }
        if(DataServerSocket != null) {
            DataServerSocket.close();
            DataServerSocket = null;
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
        while ((bytesRead = FTPInStream.read(buf, 0, WRBUF_SIZE)) > 0) {
            FTPOutStream.write(buf, 0, bytesRead);
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
        while ((bytesRead = FTPInStream.read(buf, 0, WRBUF_SIZE)) > 0) {
            FTPOutStream.write(buf, 0, bytesRead);
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
        while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
            FTPOutStream.write(buf, 0, bytesRead);
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
        while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
            FTPOutStream.write(buf, 0, bytesRead);
        }
        FTPOutStream.flush();
        if (!LocalFile.equals(""))	//We can NOT close the stdout.
            FTPOutStream.close();
        FTPInStream.close();
    }
    
    public String GetListActive() throws Exception{
        int bytesRead;
        byte [] buf = new byte[RDBUF_SIZE];
        if (Mode!=0) {
            SetStatus(2);
            throw new XylFTPException("You are not in active mode!");
        }
        
        ByteArrayOutputStream bArrayStream = new ByteArrayOutputStream();
        FTPOutStream = new BufferedOutputStream(bArrayStream);
        
        FTPInStream = new BufferedInputStream(DataSocket.getInputStream());
        while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
            FTPOutStream.write(buf, 0, bytesRead);
        }
        FTPOutStream.flush();
        FTPOutStream.close();
        FTPInStream.close();
        
        return bArrayStream.toString();
    }
    
    public String GetListPassive() throws Exception{
        int bytesRead;
        byte [] buf = new byte[RDBUF_SIZE];
        if (Mode!=1) {
            SetStatus(2);
            throw new XylFTPException("You are not in passive mode!");
        }
        
        ByteArrayOutputStream bArrayStream = new ByteArrayOutputStream();
        FTPOutStream = new BufferedOutputStream(bArrayStream);
        
        FTPInStream = new BufferedInputStream(DataSocket.getInputStream());
        while ((bytesRead = FTPInStream.read(buf, 0, RDBUF_SIZE)) > 0) {
            FTPOutStream.write(buf, 0, bytesRead);
        }
        FTPOutStream.flush();
        FTPOutStream.close();
        FTPInStream.close();
        
        return bArrayStream.toString();
    }
}
