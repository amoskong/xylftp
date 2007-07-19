class testXylTelnet{
	public static void main(String args[]){
		if(args.length !=1){
			System.out.println("Wrong usage!");
			System.exit(1);
		}
		XylTelnetConnection test = new XylTelnetConnection();
		XylFTPConnection	test2=new XylFTPConnection();
		try{
			System.out.println("The old hostname :"+test.GetHost());
			System.out.println("##SetHost to "+args[0]);
			test.SetHost(args[0]);
			System.out.println("The new hostname :"+test.GetHost());
			System.out.println("The old port :"+test.GetPort());
			System.out.println("##SetPort to 2121");
			test.SetPort(2121);
			System.out.println("The new port :"+test.GetPort());
			test.SetPort(21);
			System.out.println("The old status :"+test2.GetStatus());
			System.out.println("##Open connection");
			test.OpenConnection();
			System.out.println(test.GetEcho());
			String myip = test.GetIP();
			System.out.println("##Get my ip: "+myip);
			test2.SetStatus(1);
			System.out.println("##Send command 'SYST'");
			test.SendCommand("SYST");
			System.out.println(test.GetEcho());
			System.out.println("The new status :"+test2.GetStatus());
			System.out.println("##Close connection");
			test.CloseConnection();
			test2.SetStatus(0);
			System.out.println("The last status :"+test2.GetStatus());
		}catch(Exception e){
			System.out.println("Failed:");
			System.out.println(e.getMessage());
		}
	}
}
