import java.net.*;

// Of course, this is NOT complete. Please feel free to finish this. ;)
// President King, can you do this favor for us? 8)
// When you run this code, do please use "ftp.sjtu.edu.cn" to test. To know why, read the code please.
// AND I am afraid that there is still a bug in this code or XylFTPConnection.java. Very bad!
// When I use this for test, the file I down from ftp.sjtu.edu.cn is NOT always right. Please fix it!

class testXylFTP{
	private static String ParseIP(String From) throws Exception{
		int i, j;
		String t;
		String [] nums;
		i = From.indexOf("(");
		j = From.indexOf(")");
		t = From.substring(i+1, j);
		nums = t.split(",");
		return (nums[0]+"."+nums[1]+"."+nums[2]+"."+nums[3]);

	}
	private static int ParsePort(String From) throws Exception{
		int i, j;
		int port1, port2;
		String t;
		String [] nums;
		i = From.indexOf("(");
		j = From.indexOf(")");
		t = From.substring(i+1, j);
		nums = t.split(",");
		port1 = Integer.parseInt(nums[4]);
		port2 = Integer.parseInt(nums[5]);
		return port1*256 + port2;
	}
	public static void main(String args[]){
		if(args.length !=1){
			System.out.println("Wrong usage!");
			System.exit(1);
		}
		XylFTPConnection test = new XylFTPConnection();
		String echo;
		int i,j, port1, port2;
		String t;
		String []nums;
		try{
			test.SetHost(args[0]);
			test.SetPort(21);
			test.OpenConnection();
			test.SendCommand("SYST");
			System.out.println(test.GetEcho());
			test.SendCommand("USER anonymous");
			System.out.println(test.GetEcho());
			test.SendCommand("PASS xylftpuser@xylftp.com");
			System.out.println(echo=test.GetEcho());
			System.out.println(echo=test.GetEcho());
			while(echo.charAt(3)=='-'){
				System.out.println(echo);
				echo=test.GetEcho();
			}
			/*
			InetAddress addr = InetAddress.getLocalHost();
			byte[] ipAddr = addr.getAddress();
			test.SendCommand("PORT "+(((int)ipAddr[0])&0xff)+","+(((int)ipAddr[1])&0xff)+","+(((int)ipAddr[2])&0xff)+","+(((int)ipAddr[3])&0xff)+","+"40,44");
			System.out.println(test.GetEcho());
			test.SendCommand("LIST");
			System.out.println((echo=test.GetEcho()));
			i = echo.indexOf("(");
			j = echo.indexOf(")");
			t = echo.substring(i+1, j);
			//System.out.println("t="+t);
			nums = t.split(",");
			port1 = Integer.parseInt(nums[4]);
			port2 = Integer.parseInt(nums[5]);
			//test.SetPassive();
			test.SetActive();
			test.SetLocalFile("");
			//test.GetFile(args[0], port1*256+port2);
			test.GetFile(40*256+44);
			System.out.println(test.GetEcho());
			System.out.println(test.GetEcho());
			*/

			
			System.out.println("Test of 'ls'");
			test.SetLocalFile("");
			test.SendCommand("PASV");
			System.out.println(echo=test.GetEcho());			
			test.SetPassive();
			System.out.println(ParseIP(echo)+":"+ParsePort(echo));
			test.OpenDataConnection(ParseIP(echo), ParsePort(echo));
			test.SendCommand("LIST");
			System.out.println(test.GetEcho());			
			test.GetFilePassive();	
			test.CloseDataConnection();
			test.SetStatus(2);
			System.out.println(test.GetEcho());

			//Get a real file./////////////
			System.out.println("Test of 'get filename'");
			test.SendCommand("PASV");
			System.out.println(echo=test.GetEcho());
			test.SetHost(args[0]);
			test.SetPort(ParsePort(echo));
			test.OpenDataConnection(ParseIP(echo), ParsePort(echo));
			test.SendCommand("RETR HEADER.html");
			System.out.println((echo=test.GetEcho()));
			test.SetPassive();
			test.SetLocalFile("ttt.html");
			test.GetFilePassive();
			test.CloseDataConnection();
			System.out.println(test.GetEcho());

			//Put a real file.//////////////
			System.out.println("test of put file");
			test.CloseConnection();
			test.SetHost("kongjianjun.512j.com");
			test.SetPort(21);
			test.OpenConnection();
			test.SendCommand("USER kongjianjun");
			System.out.println(test.GetEcho());
			test.SendCommand("PASS kongjianjun");
			System.out.println(test.GetEcho());
			test.SendCommand("PASV");
			System.out.println(test.GetEcho());
			test.SendCommand("STOR ttt.html");
			System.out.println((echo=test.GetEcho()));
			test.SetPassive();
			test.SetLocalFile("ttt.html");
			test.OpenDataConnection("kongjianjun.512j.com",ParsePort(echo));
			test.SendFilePassive();
			test.CloseDataConnection();
			System.out.println(test.GetEcho());

			test.SendCommand("QUIT");
			System.out.println(test.GetEcho());
			test.CloseConnection();
		}catch(Exception e){
			System.out.println("Failed:");
			System.out.println(e.getMessage());
		}
	}
}
