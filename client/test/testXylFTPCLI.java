////////////////////////////////////////////
// Copyright (C) Kong Jian-Jun, May 12, 2007
// GPLv2 applies.
////////////////////////////////////////////
import java.net.*;

class testXylFTPCLI{
	public static void main(String args[])	throws Exception{
		String[] cmds;
		String[] cmds2;
		int i;
		XylFTPCLI test = new XylFTPCLI();

		System.out.println("The old name is :"+(test.GetUserName()));
		test.SetUserName("kongjianjun");
		System.out.println("##Set username to 'kongjianjun'");
		System.out.println("The new name is :"+(test.GetUserName()));	

		System.out.println("The old password is :"+(test.GetPassWord()));
		test.SetPassWord("NEWPASS");
		System.out.println("##Set password to 'NEWPASS'");
		System.out.println("The new password is :"+(test.GetPassWord()));	

		System.out.println("##MakeCommands of 'user\\r\\nkongjianjun\\r\\nkongjianjun\\r\\n'");
		cmds=test.MakeCommands("user\r\nkongjianjun\r\nkongjianjun\r\n");
		for(i=0;i<cmds.length;i++){
			System.out.println("cmds["+i+"]: "+cmds[i]+"\t");
		}

		System.out.println("Test of GetInput,please input any words");
		System.out.println("The content of input is :"+test.GetInput());

		System.out.println("Test of ProcessEcho");
		System.out.println("Input the content of echo :\nreturn 0;	//Invalid format.\nreturn 1;	//We need data connection.\nreturn 2;	//Don't to be continued.\nreturn 3;	//Continue.\nreturn 4;	//Send again.\nreturn 5;	//Stop resending.\nreturn 6;	//This means multi-line echo.return -1;	//other");
		System.out.println("The answer of ProcessEcho is :"+test.ProcessEcho(test.GetInput()));

		/*System.out.println("Connect to kongjianjun.512j.com\nWait…………");
		test.SetHost("kongjianjun.512j.com");
		test.SetPort(21);
		test.OpenConnection();
		System.out.println(test.GetEcho());
		test.SendCommand("USER kongjianjun");
		System.out.println(test.GetEcho());
		test.SendCommand("PASS kongjianjun");
		System.out.println(test.GetEcho());*/
		test.SetStatus(2);
		System.out.println("Test of GetCommands");
		System.out.println("The answer of GetCommands is :");
		cmds2=test.GetCommands();
		for(i=0;i<cmds2.length;i++){
			System.out.println("cmds2["+i+"]: "+cmds2[i]+"\t");
		}

	}
}
